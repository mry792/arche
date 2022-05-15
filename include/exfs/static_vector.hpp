#ifndef EXFS_STATIC_VECTOR_HPP_
#define EXFS_STATIC_VECTOR_HPP_

#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <type_traits>

#include "exfs/iterator/concepts.hpp"
#include "exfs/iterator/reverse_iterator.hpp"
#include "exfs/memory/storage.hpp"
#include "exfs/utility/functions.hpp"

namespace exfs {
/**
 * Contiguous container with a variable size but pre-allocated memory.
 *
 * The elements of this container are stored contiguously, which means that
 * elements can be accessed not only through iterators, but also using offsets
 * to regular pointers to elements. This means that a pointer to an element of
 * a @c static_vector may be passed to any function that expected a pointer to
 * an element of an array.
 *
 * This is based on the paper p0843r2 which proposes this for standardization.
 * it matches the interface of @c std::vector as closely as possible. Main
 * exceptions include a lack of a custom allocator and the ability to control
 * reserved memory.
 *
 * http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0843r2.html
 */
template <typename T, std::size_t N>
class static_vector {
    template <typename U>
    using rev_iter_ = exfs::iterator::reverse_iterator<U>;

  public:
    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = T const*;
    using reference       = value_type&;
    using const_reference = value_type const&;
    using size_type       = std::size_t;
    using difference_type = std::make_signed_t<size_type>;

    using iterator               = pointer;
    using const_iterator         = const_pointer;
    using reverse_iterator       = rev_iter_<iterator>;
    using const_reverse_iterator = rev_iter_<const_iterator>;

  private:
    using storage_type_ = exfs::memory::storage<T>;
    storage_type_ storage_[N];
    size_type size_ = 0u;

  public:
    /**
     * @name Constructors
     * @{
     */

    /**
     * Default constructor. Constructs an empty container.
     */
    constexpr static_vector () noexcept = default;

    /**
     * Constructs the container with @p count default-inserted instances of @p
     * T. No copies are made.
     *
     * @warning It is undefined behavior if @p count is more than the static
     *     capacity of the container.
     *
     * @param[in] count The number of elements to construct in the initial
     *     container.
     */
    constexpr explicit static_vector (size_type count)
    noexcept(std::is_nothrow_default_constructible_v<T>) : size_{count} {
        for (size_type idx = 0u; idx < size_; ++idx) {
            storage_[idx].construct();
        }
    }

    /**
     * Constructs the container with @p count copies of elements with value @p
     * value.
     *
     * @warning It is undefined behavior if @p count is more than the static
     *     capacity of the container.
     *
     * @param[in] count The number of elements to construct in the initial
     *     container.
     * @param[in] value The value with which to initialize the elements.
     */
    constexpr static_vector (size_type count, const value_type& value)
    noexcept(std::is_nothrow_constructible_v<T, T const&>) : size_{count} {
        for (size_type idx = 0u; idx < size_; ++idx) {
            storage_[idx].construct(value);
        }
    }

    /**
     * Constructs the container with the contents of the range
     * [first, sentinel).
     *
     * @warning It is undefined behavior if `distance(first, sentinel)` is
     *     greater than the static capacity of the container.
     *
     * @param[in] first The first item in the range to copy.
     * @param[in] sentinel The sentinel value of the range.
     */
    template <
        exfs::iterator::input_iterator Iterator,
        exfs::iterator::sentinel_for<Iterator> Sentinel
    >
    constexpr static_vector (Iterator first, Sentinel sentinel)
    noexcept(std::is_nothrow_constructible_v<T, decltype(*first)>) {
        for (size_ = 0u; first != sentinel; ++size_, ++first) {
            storage_[size_].construct(*first);
        }
    }

    /**
     * Constructs the container with the copy of the contents of @p other.
     *
     * @param[in] other The other container to copy from.
     */
    constexpr static_vector (static_vector const& other)
    noexcept(std::is_nothrow_copy_constructible_v<value_type>)
          : size_{other.size_} {
        for (size_type idx = 0u; idx < size_; ++idx) {
            storage_[idx].construct(other.storage_[idx].object());
        }
    }

    /**
     * Constructs the container with the contents of other using move semantics.
     * After the move, @p other is guaranteed to be @p empty().
     *
     * @param[in] other The other container to move from.
     */
    constexpr static_vector (static_vector&& other)
    noexcept(std::is_nothrow_move_constructible_v<value_type>)
          : size_{other.size_} {
        for (size_type idx = 0u; idx < size_; ++idx) {
            storage_[idx].construct(exfs::move(other.storage_[idx]).object());
            other.storage_[idx].destroy();
        }

        other.size_ = 0u;
    }

    /**
     * Constructs the container with the contents of the initializer list @p
     * init.
     *
     * @param[in] init The initializer list to copy from.
     */
    constexpr static_vector (std::initializer_list<value_type> init)
          : size_{init.size()} {
        auto iter = init.begin();
        auto const end = init.end();

        size_type idx = 0u;
        while (iter != end) {
            storage_[idx++].construct(*iter++);
        }
    }

    /**
     * @}
     */

    /**
     * @name Destructor
     * @{
     */

    /**
     * Destructs the @c static_vector. The destructors of the elements are
     * called. Note, that if the elements are pointers, the pointed-to objects
     * are not destroyed.
     */
    constexpr ~static_vector () {
        destroy_all_();
    }

    /**
     * @}
     */

    /**
     * @name Assignment
     * @{
     */

    /**
     * Copy assignment operator. Replaces the contents with a copy of the
     * contents of other.
     * @param[in] other The container object to copy from.
     * @return A reference to the current container object.
     */
    constexpr static_vector& operator = (static_vector const& other)
    noexcept(
        std::is_nothrow_copy_assignable_v<value_type> and
        std::is_nothrow_copy_constructible_v<value_type>
    ) {
        // This implementation assumes that copy-assigning T is cheaper than T's
        // destructor + copy-constructor. The approach here is to iterate over
        // the two containers in lock-step doing the minimal action required.
        size_type idx = 0u;

        // First, copy-assign where elements exist in both containers.
        for (; idx < size_ and idx < other.size_; ++idx) {
            storage_[idx].object() = other.storage_[idx].object();
        }

        // Then, copy-construct where other has more elements.
        for (; idx < other.size_; ++idx) {
            storage_[idx].construct(other.storage_[idx].object());
        }

        // Finally, destruct any remaining elements of this container.
        for (; idx < size_; ++idx) {
            storage_[idx].destroy();
        }

        size_ = other.size_;
        return *this;
    }

    /**
     * Move assignment operator. Replaces the contents with those of @p other
     * using element-wise move construction/assignment. After this call, @p
     * other will be empty.
     * @param[in] other The container object to move from.
     * @return A reference to the current container object.
     */
    constexpr static_vector& operator = (static_vector&& other)
    noexcept(
        std::is_nothrow_move_assignable_v<value_type> and
        std::is_nothrow_move_constructible_v<value_type>
    ) {
        // This implementation assumes that move-assigning T is cheaper than T's
        // destructor + move-constructor. The approach here is to iterator over
        // the two containers in lock-step doing the minimal action required.
        size_type idx = 0u;

        // First, move-assign where elements exist in both containers.
        for (; idx < size_ and idx < other.size_; ++idx) {
            storage_[idx].object() = exfs::move(other.storage_[idx]).object();
        }

        // Then, move-construct where other has more elements.
        for (; idx < other.size_; ++idx) {
            storage_[idx].construct(exfs::move(other.storage_[idx]).object());
        }

        // Next, destruct any remaining elements of this container.
        for (; idx < size_; ++idx) {
            storage_[idx].destroy();
        }

        // Lastly, destruct all elements of `other`.
        other.destroy_all_();

        size_ = exfs::exchange(other.size_, 0u);
        return *this;
    }

    // template <class InputIterator>
    // constexpr void assign(InputIterator first, InputIterator last);
    // constexpr void assign(size_type n, const value_type& u);
    // constexpr void assign(initializer_list<value_type> il);

    /**
     * @}
     */

    /**
     * @name Iterators
     * @{
     */

    // constexpr iterator               begin()         noexcept;
    // constexpr const_iterator         begin()   const noexcept;
    // constexpr const_iterator         cbegin()  const noexcept;
    // constexpr iterator               end()           noexcept;
    // constexpr const_iterator         end()     const noexcept;
    // constexpr const_iterator         cend()    const noexcept;
    // constexpr reverse_iterator       rbegin()        noexcept;
    // constexpr const_reverse_iterator rbegin()  const noexcept;
    // constexpr const_reverse_iterator crbegin() const noexcept;
    // constexpr reverse_iterator       rend()          noexcept;
    // constexpr const_reverse_iterator rend()    const noexcept;
    // constexpr const_reverse_iterator crend()   const noexcept;

    /**
     * @}
     */

    /**
     * @name Size/Capacity
     * @{
     */

    /**
     * Checks if the container has no elements, i.e. whether `begin() == end()`.
     * @return @c true if the container is empty, @c false otherwise.
     */
    [[nodiscard]] constexpr bool empty () const noexcept {
        return size_ == 0u;
    }

    /**
     * Returns the number of elements in the container.
     * @return The number of elements in the container.
     */
    [[nodiscard]] constexpr size_type size () const noexcept {
        return size_;
    }

    // static constexpr size_type max_size() noexcept;
    // static constexpr size_type capacity() noexcept;
    // constexpr void resize(size_type sz);
    // constexpr void resize(size_type sz, const value_type& c);

    /**
     * @}
     */

    /**
     * @name Element and Data Access
     * @{
     */

    /**
     * Returns a reference to the element at specified location. No bounds
     * checking is performed.
     *
     * @warning Accessing a nonexistent element is undefined behavior.
     *
     * @param[in] pos The position of the element to return.
     *
     * @return Reference to the requested element.
     */
    constexpr reference operator [] (size_type pos) {
        return storage_[pos].object();
    }

    /**
     * @overload operator[]()
     */
    constexpr const_reference operator [] (size_type pos) const {
        return storage_[pos].object();
    }

    /**
     * Access the first element.
     *
     * @warning Calling @c front() on an empty container is undefined.
     *
     * @return A reference to the first element in the container.
     */
    constexpr reference front () {
        return this->operator[](0);
    }

    /**
     * @overload front()
     */
    constexpr const_reference front () const {
        return this->operator[](0);
    }

    /**
     * Access the last element.
     *
     * @warning Calling @c back() on an empty container is undefined.
     *
     * @return A reference to the last element in the container.
     */
    constexpr reference back () {
        return this->operator[](size_ - 1);
    }

    /**
     * @overload back()
     */
    constexpr const_reference back () const {
        return this->operator[](size_ - 1);
    }

    // constexpr       T* data()       noexcept;
    // constexpr const T* data() const noexcept;

    /**
     * @}
     */

    /**
     * @name Modifiers
     * @{
     */

    // constexpr iterator insert(const_iterator position, const value_type& x);
    // constexpr iterator insert(const_iterator position, value_type&& x);
    // constexpr iterator insert(const_iterator position, size_type n, const value_type& x);
    // template <class InputIterator>
    // constexpr iterator insert(const_iterator position, InputIterator first, InputIterator last);
    // constexpr iterator insert(const_iterator position, initializer_list<value_type> il);

    // template <class... Args>
    // constexpr iterator emplace(const_iterator position, Args&&... args);

    /**
     * Appends a new element to the end of the container. The element is
     * constructed through the value type's constructor that corresponds to the
     * forwarded arguments.
     *
     * @warning Calling this function when the container is full is undefined
     *     behavior.
     *
     * @param[in,out] args... Arguments to forward to the constructor of the
     *     element.
     *
     * @return A reference to inserted element.
     */
    template <typename... Args>
    constexpr reference emplace_back (Args&&... args)
    noexcept(std::is_nothrow_constructible_v<value_type, Args...>) {
        auto& elem_storage = storage_[size_];
        elem_storage.construct(exfs::forward<Args>(args)...);
        ++size_;
        return elem_storage.object();
    }

    /**
     * Appends the given element value to the end of the container. The new
     * element is initialized from the forwarded @p value.
     *
     * @warning Calling this function when the container is full is undefined
     *     behavior.
     *
     * @param[in] value The value of the element to append.
     */
    template <typename U>
    requires (std::same_as<std::remove_cvref_t<U>, value_type>)
    constexpr void push_back (U&& value)
    noexcept(std::is_nothrow_constructible_v<value_type, U>) {
        storage_[size_].construct(exfs::forward<U>(value));
        ++size_;
    }

    // constexpr void pop_back();
    // constexpr iterator erase(const_iterator position);
    // constexpr iterator erase(const_iterator first, const_iterator last);

    /**
     * Erases all elements from the container. After this call, `size()` returns
     * zero. Invalidates any references, pointers, or iterators referring to
     * contained elements. Any past-the-end iterators are also invalidated.
     */
    constexpr void clear () noexcept {
        destroy_all_();
        size_ = 0u;
    }

    /**
     * @}
     */

  private:
    constexpr void destroy_all_ () noexcept {
        for (size_type idx = 0u; idx < size_; ++idx) {
            storage_[idx].destroy();
        }
    }
};
}  // namespace exfs

#endif  // EXFS_STATIC_VECTOR_HPP_

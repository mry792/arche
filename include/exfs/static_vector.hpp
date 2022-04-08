#ifndef EXFS_STATIC_VECTOR_HPP_
#define EXFS_STATIC_VECTOR_HPP_

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

    // constexpr static_vector& operator=(const static_vector& other)
    //     noexcept(is_nothrow_copy_assignable_v<value_type>);
    // constexpr static_vector& operator=(static_vector&& other);
    //     noexcept(is_nothrow_move_assignable_v<value_type>);
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

    // constexpr reference       operator[](size_type n);
    // constexpr const_reference operator[](size_type n) const;
    // constexpr reference       front();
    // constexpr const_reference front() const;
    // constexpr reference       back();
    // constexpr const_reference back() const;
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
    // template <class... Args>
    // constexpr reference emplace_back(Args&&... args);
    // constexpr void push_back(const value_type& x);
    // constexpr void push_back(value_type&& x);

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

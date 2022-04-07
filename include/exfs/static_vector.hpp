#ifndef EXFS_STATIC_VECTOR_HPP_
#define EXFS_STATIC_VECTOR_HPP_

#include <cstddef>
#include <type_traits>

#include "exfs/iterator/reverse_iterator.hpp"
#include "exfs/memory/storage.hpp"

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

    // constexpr static_vector(size_type n, const value_type& value);
    // template <class InputIterator>
    // constexpr static_vector(InputIterator first, InputIterator last);
    // constexpr static_vector(const static_vector& other)
    //     noexcept(is_nothrow_copy_constructible_v<value_type>);
    // constexpr static_vector(static_vector&& other)
    //     noexcept(is_nothrow_move_constructible_v<value_type>);
    // constexpr static_vector(initializer_list<value_type> il);

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
        for (size_type idx = 0u; idx < size_; ++idx) {
            storage_[idx].destroy();
        }
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

    // constexpr void clear() noexcept;

    /**
     * @}
     */
};
}  // namespace exfs

#endif  // EXFS_STATIC_VECTOR_HPP_

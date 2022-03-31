#ifndef EXFS_ITERATOR_REVERSE_ITERATOR_HPP_
#define EXFS_ITERATOR_REVERSE_ITERATOR_HPP_

#include <concepts>
#include <type_traits>

#include "exfs/iterator/category_tags.hpp"
#include "exfs/iterator/concepts.hpp"
#include "exfs/iterator/operations.hpp"
#include "exfs/iterator/traits.hpp"
#include "exfs/utility/functions.hpp"

namespace exfs::iterator {
/**
 * This class is an iterator adaptor that reverses the direction of a given
 * iterator, which must model @c bidirectional_iterator. In other words, when
 * provided with a bidirectional iterator, @c reverse_iterator produces a new
 * iterator that moves from the end to the beginning of the sequence defined by
 * the underlying bidirectional iterator.
 *
 * For a reverse iterator @c r constructed from an iterator @c i, the
 * relationship `&*r == &*(i - 1)` is always true (as long as @c r is
 * dereferenceable); thus a reverse iterator constructed from a
 * one-past-the-end iterator dereferences to the last element in a sequence.
 *
 * This is the iterator returned by member functions `rbegin()` and @c `rend()`
 * of the standard library containers.
 */
template <bidirectional_iterator Iter>
class reverse_iterator {
  public:
    using iterator_type     = Iter;
    using iterator_concept  = std::conditional_t<
        random_access_iterator<Iter>,
        random_access_iterator_tag,
        bidirectional_iterator_tag
    >;
    using iterator_category = std::conditional_t<
        std::is_base_of_v<
            random_access_iterator_tag,
            iterator_category_t<Iter>
        >,
        random_access_iterator_tag,
        iterator_category_t<Iter>
    >;
    using value_type        = iter_value_t<Iter>;
    using difference_type   = iter_difference_t<Iter>;
    using pointer           = typename iterator_traits<Iter>::pointer;
    using reference         = iter_reference_t<Iter>;

    /**
     * Default constructor.
     *
     * The underlying iterator is value-initialized. Operations on the resulting
     * iterator have defined behavior if and only if the corresponding
     * operations on a value-initialized @p Iter also have defined behavior.
     */
    constexpr reverse_iterator () : base_{} {}

    /**
     * Construct a @c reverse_iterator wrapping @p iter.
     *
     * @param[in] iter The underlying iterator to adapt.
     */
    constexpr explicit reverse_iterator (iterator_type iter)
          : base_{exfs::move(iter)} {}

    /**
     * Construct a @c reverse_iterator wrapping a converted copy of @p other.
     *
     * This overload participates in overload resolution only if @p Other is not
     * the same type as @p Iter and `std::convertible_to<Other const&, Iter>`
     * is modeled.
     *
     * @param[in] other Iterator to convert and adapt.
     */
    template <typename Other>
    requires (
        not std::is_same_v<Other, Iter> and
        std::convertible_to<Other const&, Iter>
    )
    constexpr reverse_iterator (reverse_iterator<Other> const& other)
          : base_{other.base()} {}

    /**
     * Construct a @c reverse_iterator wrapping a converted move of @p other.
     *
     * This overload participates in overload resolution only if @p Other is not
     * the same type as @p Iter and `std::convertible_to<Other&&, Iter>`
     * is modeled.
     *
     * @param[in] other Iterator to convert and adapt.
     */
    template <typename Other>
    requires (
        not std::is_same_v<Other, Iter> and
        std::convertible_to<Other&&, Iter>
    )
    constexpr reverse_iterator (reverse_iterator<Other>&& other)
          : base_{exfs::move(other.base())} {}

    /**
     * Copy-assigns the underlying iterator from that of @p other.
     *
     * @param[in] other The iterator adaptor to assign from.
     */
    template <typename Other>
    requires (
        not std::is_same_v<Other, Iter> and
        std::convertible_to<Other const&, Iter> and
        std::assignable_from<iterator_type&, Other const&>
    )
    constexpr reverse_iterator&
    operator = (reverse_iterator<Other> const& other) {
        base_ = other.base();
        return *this;
    }

    /**
     * Move-assigns the underlying iterator from that of @p other.
     *
     * @param[in] other The iterator adaptor to assign from.
     */
    template <typename Other>
    requires (
        not std::is_same_v<Other, Iter> and
        std::convertible_to<Other&&, Iter> and
        std::assignable_from<iterator_type&, Other&&>
    )
    constexpr reverse_iterator& operator = (reverse_iterator<Other>&& other) {
        base_ = std::move(other.base());
        return *this;
    }

    /**
     * Access the underlying base iterator.
     *
     * This should be the same iterator that this was constructed with. That is
     * `reverse_iterator(it).base() == it`.
     *
     * The base iterator refers to the element that is next (from the
     * `reverse_iterator::iterator_type` perspective) to the element the @c
     * reverse_iterator is currently pointing to. That is `&*(rit.base() - 1)
     * == &*rit`.
     *
     * @except May throw implementation-defined exceptions.
     * @return The underlying iterator.
     */
    constexpr iterator_type base () const { return base_; }

    /**
     * Returns a reference to the "current" element of the reverse iterator.
     *
     * The "current" element is the previous element of the base iterator. The
     * element is mutable if the underlying iterator type provides mutable
     * access.
     *
     * @return A reference to the "current" element.
     */
    constexpr reference operator * () const {
        return *exfs::iterator::prev(base_);
    }

    /**
     * Returns a pointer to the "current" element of the reverse iterator.
     *
     * The "current" element is the previous element of the base iterator. The
     * pointer is mutable if the underlying iterator type provides mutable
     * access.
     *
     * @return A pointer to the "current" element.
     */
    constexpr pointer operator -> () const
    requires (
        std::is_pointer_v<iterator_type> or
        requires (iterator_type const i) {
            i.operator->();
        }
    ) {
        if constexpr (std::is_pointer_v<iterator_type>) {
            return base_ - 1;
        } else {
            return exfs::iterator::prev(base_).operator->();
        }
    }

    /**
     * Returns a reference to the element at the specified relative location.
     *
     * @param[in] idx Position relative to current location.
     *
     * @return A reference to the elemtn at relative location, that is, `base()
     *     [-n - 1]`.
     */
    constexpr reference operator [] (difference_type idx) const
    requires (random_access_iterator<Iter>) {
        return base()[-1 - idx];
    }

    /**
     * Pre-increments the iterator by one.
     */
    constexpr reverse_iterator& operator ++ () {
        --base_;
        return *this;
    }

    /**
     * Pre-decrements the iterator by one.
     */
    constexpr reverse_iterator& operator -- () {
        ++base_;
        return *this;
    }

    /**
     * Post-increments the iterator by one.
     */
    constexpr reverse_iterator operator ++ (int) {
        return reverse_iterator{base_--};
    }

    /**
     * Post-decrements the iterator by one.
     */
    constexpr reverse_iterator operator -- (int) {
        return reverse_iterator{base_++};
    }

    /**
     * Increments the iterator by the given distance.
     *
     * The base iterator @p Iter must be a random access iterator.
     *
     * @param[in] rev The reverse iterator to advance.
     * @param[in] dist The amount to advance it by.
     *
     * @return The new iterator after advancing.
     */
    friend constexpr reverse_iterator operator + (
        reverse_iterator const& rev,
        difference_type dist
    ) requires (random_access_iterator<Iter>) {
        return reverse_iterator{rev.base() - dist};
    }

    /**
     * Increments the iterator by the given distance.
     *
     * The base iterator @p Iter must be a random access iterator.
     *
     * @param[in] dist The amount to advance it by.
     * @param[in] rev The reverse iterator to advance.
     *
     * @return The new iterator after advancing.
     */
    friend constexpr reverse_iterator operator + (
        difference_type dist,
        reverse_iterator const& rev
    ) requires (random_access_iterator<Iter>) {
        return reverse_iterator{rev.base() - dist};
    }

    /**
     * Decrements the iterator by the given distance.
     *
     * The base iterator @p Iter must be a random access iterator.
     *
     * @param[in] rev The reverse iterator to recede.
     * @param[in] dist The amount to recede it by.
     *
     * @return The new iterator after receding.
     */
    friend constexpr reverse_iterator operator - (
        reverse_iterator const& rev,
        difference_type dist
    ) requires (random_access_iterator<Iter>) {
        return reverse_iterator{rev.base() + dist};
    }

    /**
     * Computes the distance between the two iterator adaptors.
     *
     * @param[in] lhs
     * @param[in] rhs
     *
     * @return The computed distance.
     */
    friend constexpr difference_type operator - (
        reverse_iterator const& lhs,
        reverse_iterator const& rhs
    ) requires (random_access_iterator<Iter>) {
        return rhs.base() - lhs.base();
    }

  private:
    iterator_type base_;
};
}  // exfs::iterator

#endif  // EXFS_ITERATOR_REVERSE_ITERATOR_HPP_

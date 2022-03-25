#ifndef EXFS_ITERATOR_OPERATIONS_HPP_
#define EXFS_ITERATOR_OPERATIONS_HPP_

#include "exfs/iterator/legacy.hpp"
#include "exfs/iterator/traits.hpp"

namespace exfs::iterator {
/**
 * Increments given iterator @p iter by @p dist elements.
 *
 * If @p dist is negative, the iterator is decremented. In this case, @p Iter
 * must meet the requirements of @c legacy_bidirectional_iterator, otherwise
 * the behavior is undefined.
 *
 * @note The behavior is undefined if the specified sequence of increments or
 *     decrements would require that a non-incrementable iterator (such as a
 *     past-the-end iterator) is incremented, or that a non-decrementable
 *     iterator (such as the front iterator or the singular iterator) is
 *     decremented.
 *
 * @param[in,out] iter The iterator to be advanced.
 * @param[in,out] dist The number of @p iter should be advanced.
 */
template <legacy_input_iterator Iter>
constexpr void advance (
    Iter& iter,
    typename incrementable_traits<Iter>::difference_type dist
) {
    if constexpr (legacy_random_access_iterator<Iter>) {
        iter += dist;
    } else {
        while (dist > 0) {
            --dist;
            ++iter;
        }
        if constexpr (legacy_bidirectional_iterator<Iter>) {
            while (dist < 0) {
                ++dist;
                --iter;
            }
        }
    }
}

/**
 * Returns the number of hops from @p first to @p last.
 *
 * @note If @p Iter does not satisfy @c legacy_random_access_iterator, the
 *     behavior is undefined if @p last is not reachable from @p first by
 *     (possibly repeatedly) incrementing @p first. If @p Iter does satisfy @c
 *     legacy_random_access_iterator, the behavior is undefined if @p last is
 *     not reachable from @p first and @p first is not reachable from @p last.
 *
 * @param[in] first Iterator pointing to the first element.
 * @param[in] last Iterator pointing to the end of the range.
 *
 * @return The number of increments needed to go from @p first to @p last. The
 *     value may be negative if random-access iterators are used and @p first is
 *     reachable from @p last.
 */
template <legacy_input_iterator Iter>
constexpr
typename incrementable_traits<Iter>::difference_type
distance (Iter first, Iter last) {
    if constexpr (legacy_random_access_iterator<Iter>) {
        return last - first;
    } else {
        using Distance = typename incrementable_traits<Iter>::difference_type;
        Distance result = 0;
        while (first != last) {
            ++first;
            ++result;
        }
        return result;
    }
}
}  // exfs::iterator

#endif  // EXFS_ITERATOR_OPERATIONS_HPP_

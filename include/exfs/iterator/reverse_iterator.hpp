#ifndef EXFS_ITERATOR_REVERSE_ITERATOR_HPP_
#define EXFS_ITERATOR_REVERSE_ITERATOR_HPP_

#include <type_traits>

#include "exfs/iterator/category_tags.hpp"
#include "exfs/iterator/concepts.hpp"
#include "exfs/iterator/traits.hpp"

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
};
}  // exfs::iterator

#endif  // EXFS_ITERATOR_REVERSE_ITERATOR_HPP_

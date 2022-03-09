#ifndef EXFS_ITERATOR_LEGACY_HPP_
#define EXFS_ITERATOR_LEGACY_HPP_

#include <concepts>

#include "exfs/concepts.hpp"
#include "exfs/iterator/traits.hpp"

namespace exfs::iterator {
/**
 * Describes types that can be used to identify and traverse the elements of a
 * container. This concept is the base set of requirements used by other
 * iterator types. Iterators can be thought of as an abstraction of pointers.
 */
template <typename I>
concept legacy_iterator =
    requires (I i) {
        {   *i } -> referenceable;
        {  ++i } -> std::same_as<I&>;
        { *i++ } -> referenceable;
    } and std::copyable<I>;

/**
 * This concept is a refinement of the @c legacy_iterator concept that adds the
 * ability to read from the pointed-to element. They only guarantee validity
 * for single pass algorithms; once a @c legacy_input_iterator has been
 * incremented, all copies of its previous value may be invalidated.
 */
template <typename I>
concept legacy_input_iterator =
    legacy_iterator<I> and
    std::equality_comparable<I> and
    requires (I i) {
        typename incrementable_traits<I>::difference_type;
        typename indirectly_readable_traits<I>::value_type;
        typename std::common_reference_t<
            iter_reference_t<I>&&,
            typename indirectly_readable_traits<I>::value_type&
        >;
        *i++;
        typename std::common_reference_t<
            decltype(*i++)&&,
            typename indirectly_readable_traits<I>::value_type&
        >;
        requires std::signed_integral<
            typename incrementable_traits<I>::difference_type>;
    };
}  // exfs::iterator

#endif  // EXFS_ITERATOR_LEGACY_HPP_

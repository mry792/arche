#ifndef EXFS_ITERATOR_LEGACY_HPP_
#define EXFS_ITERATOR_LEGACY_HPP_

#include <concepts>

#include "exfs/concepts.hpp"

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
}  // exfs::iterator

#endif  // EXFS_ITERATOR_LEGACY_HPP_

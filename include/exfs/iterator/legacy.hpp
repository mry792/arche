#ifndef EXFS_ITERATOR_LEGACY_HPP_
#define EXFS_ITERATOR_LEGACY_HPP_

#include <concepts>
#include <type_traits>

#include "exfs/concepts.hpp"
#include "exfs/iterator/traits.hpp"
#include "exfs/utility/functions.hpp"

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

/**
 * This concept is a refinement of the @c legacy_iterator concept that can write
 * to the pointed-to element by a value of type @c V. When an iterator that
 * models @c legacy_forward_iterator, @c legacy_bidirectional_iterator, or @c
 * legacy_random_access_iterator satisfies the @c legacy_output_iterator
 * concept, it is described as "mutable".
 */
template <typename I, typename V>
concept legacy_output_iterator =
    legacy_iterator<I> and
    requires (I i, V&& v) {
        *i = exfs::forward<V>(v);
        { i++ } -> std::convertible_to<I const&>;
        *i++ = exfs::forward<V>(v);
    };

/**
 * This concept describes a @c legacy_iterator that can read data from the
 * pointed-to element. Unlike @c legacy_input_iterator and @c
 * legacy_output_iterator, it can be used in multipass algorithms.
 *
 * If a @c legacy_forward_iterator originates from a container, then it's @c
 * value_type is the same as the container's.
 *
 * @warning This concept does not check that @p I provides a "multipass
 *     guarantee." This is not currently expressible with concepts.
 */
template <typename I>
concept legacy_forward_iterator =
    legacy_input_iterator<I> and
    std::constructible_from<I> and
    std::is_lvalue_reference_v<iter_reference_t<I>> and
    std::same_as<
        std::remove_cvref_t<iter_reference_t<I>>,
        typename indirectly_readable_traits<I>::value_type
    > and
    requires (I i) {
        {  i++ } -> std::convertible_to<I const&>;
        { *i++ } -> std::same_as<iter_reference_t<I>>;
    };

/**
 * A @c legacy_bidirectional_iterator is a @c legacy_forward_iterator that can
 * be moved in both directions (i.e. incremented and decremented). If a @c
 * legacy_bidirectional_iterator originates from a container, then it's @c
 * value_type is the same as the container's.
 */
template <typename I>
concept legacy_bidirectional_iterator =
    legacy_forward_iterator<I> and
    requires (I i) {
        {  --i } -> std::same_as<I&>;
        {  i-- } -> std::convertible_to<I const&>;
        { *i-- } -> std::same_as<iter_reference_t<I>>;
    };

/**
 * A @c legacy_random_access_iterator is a @c legacy_bidirectional_iterator that
 * can be moved to point to any element in constant time. If a @c
 * legacy_random_access_iterator originates from a container, then it's @c
 * value_type is the same as the container's.
 *
 * A pointer to an element of an array satisfies all requirements of @c
 * legacy_random_access_iterator.
 */
template <typename I>
concept legacy_random_access_iterator =
    legacy_bidirectional_iterator<I> and
    std::totally_ordered<I> and
    requires (I i, typename incrementable_traits<I>::difference_type n) {
        { i += n } -> std::same_as<I&>;
        { i -= n } -> std::same_as<I&>;
        { i +  n } -> std::same_as<I>;
        { n +  i } -> std::same_as<I>;
        { i -  n } -> std::same_as<I>;
        { i -  i } -> std::same_as<decltype(n)>;
        { i[n]   } -> std::convertible_to<iter_reference_t<I>>;
    };
}  // exfs::iterator

#endif  // EXFS_ITERATOR_LEGACY_HPP_

#ifndef EXFS_ITERATOR_LEGACY_HPP_
#define EXFS_ITERATOR_LEGACY_HPP_

#include <concepts>
#include <type_traits>

#include "exfs/concepts.hpp"
#include "exfs/iterator/category_tags.hpp"
#include "exfs/iterator/traits.hpp"
#include "exfs/utility/functions.hpp"

namespace exfs::iterator {
/**
 * @name legacy iterator concepts
 * @{
 */

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

/**
 * @}
 */

/**
 * @name iterator_traits
 * @{
 */

/**
 * Identify the category of the iterator.
 *
 * This trait is an extension to the standard library.
 *
 * @todo Specialization to take the @c iterator_traits<Iter>::iterator_category
 *     if @c iterator_trait is specialized for @p Iter.
 *
 * @tparam Iter Iterator type in question.
 */
template <typename Iter>
struct iterator_category {};

template <legacy_input_iterator Iter>
struct iterator_category<Iter> {
    using type = input_iterator_tag;
};

// Cannot check for a legacy_output_iterator without an extra argument.

template <legacy_forward_iterator Iter>
struct iterator_category<Iter> {
    using type = forward_iterator_tag;
};

template <legacy_bidirectional_iterator Iter>
struct iterator_category<Iter> {
    using type = bidirectional_iterator_tag;
};

template <legacy_random_access_iterator Iter>
struct iterator_category<Iter> {
    using type = random_access_iterator_tag;
};

template <typename Iter>
using iterator_category_t = typename iterator_category<Iter>::type;

namespace __detail {

/**
 * The standard library prescribes only specific specializations of @c
 * iterator_traits. To achieve the implementation variability required without
 * specialization, this template is used as a base class with the desired
 * variability as specializations.
 *
 * Primary template for @c __iter_traits has no members. (It is
 * SFINAE-friendly.)
 */
template <typename Iter>
struct __iter_traits {};

template <typename Iter>
concept __has_most_iter_trait_members = requires {
    typename Iter::difference_type;
    typename Iter::value_type;
    // typename Iter::pointer;
    typename Iter::reference;
    typename Iter::iterator_category;
};

/**
 * Specialization of @c __iter_traits for when @p Iter defines all expected
 * member types.
 */
template <typename Iter>
requires (
    __has_most_iter_trait_members<Iter> and
    requires { typename Iter::pointer; }
)
struct __iter_traits<Iter> {
    using difference_type   = typename Iter::difference_type;
    using value_type        = typename Iter::value_type;
    using pointer           = typename Iter::pointer;
    using reference         = typename Iter::reference;
    using iterator_category = typename Iter::iterator_category;
};

/**
 * Specialization of @c __iter_traits for when @p Iter defines all member types
 * except a @c pointer member type.
 */
template <typename Iter>
requires (
    __has_most_iter_trait_members<Iter> and
    not requires { typename Iter::pointer; }
)
struct __iter_traits<Iter> {
    using difference_type   = typename Iter::difference_type;
    using value_type        = typename Iter::value_type;
    using pointer           = void;
    using reference         = typename Iter::reference;
    using iterator_category = typename Iter::iterator_category;
};

template <typename Iter>
struct __safe_iter_ptr {
    using type = void;
};

template <typename Iter>
requires (requires { typename exfs::iterator::iter_pointer_t<Iter>; })
struct __safe_iter_ptr<Iter> {
    using type = exfs::iterator::iter_pointer_t<Iter>;
};

template <legacy_input_iterator Iter>
requires (not __has_most_iter_trait_members<Iter>)
struct __iter_traits<Iter> {
    using difference_type   = typename incrementable_traits<Iter>::difference_type;
    using value_type        = typename indirectly_readable_traits<Iter>::value_type;
    using pointer           = typename __safe_iter_ptr<Iter>::type;
    using reference         = iter_reference_t<Iter>;
    using iterator_category = iterator_category_t<Iter>;
};

template <typename T>
struct __safe_iter_diff {
    using type = void;
};

template <typename T>
requires (requires { typename incrementable_traits<T>::difference_type; })
struct __safe_iter_diff<T> {
    using type = typename incrementable_traits<T>::difference_type;
};

template <legacy_iterator Iter>
requires (not __has_most_iter_trait_members<Iter>)
struct __iter_traits<Iter> {
    using difference_type   = typename __safe_iter_diff<Iter>::type;
    using value_type        = void;
    using pointer           = void;
    using reference         = void;
    using iterator_category = output_iterator_tag;
};
}  // namespace __detail

/**
 * The class @c iterator_traits is the type trait that provides uniform
 * interface to the properties of @c LegacyIterator types. This makes it
 * possible to implement algorithms only in terms of iterators.
 *
 * The template can be specialized for user-defined iterators so that the
 * information about the iterator can be retrieved even if the type does not
 * provide the usual aliases.
 *
 * User specializations may define the member type @c iterator_concept to one of
 * the iterator category tags to indicate conformance to the respective
 * iterator concepts.
 */
template <typename T>
struct iterator_traits : __detail::__iter_traits<T> {};

/**
 * Specialization of @c iterator_traits for pointers.
 */
template <typename T>
requires (std::is_object_v<T>)
struct iterator_traits<T*> {
    using difference_type   = std::ptrdiff_t;
    using value_type        = std::remove_cv_t<T>;
    using pointer           = T*;
    using reference         = T&;
    using iterator_category = random_access_iterator_tag;
    using iterator_concept  = contiguous_iterator_tag;
};
}  // exfs::iterator

#endif  // EXFS_ITERATOR_LEGACY_HPP_

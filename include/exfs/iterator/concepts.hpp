#ifndef EXFS_ITERATOR_CONCEPTS_HPP_
#define EXFS_ITERATOR_CONCEPTS_HPP_

#include <concepts>
#include <type_traits>

#include "exfs/concepts.hpp"
#include "exfs/iterator/category_tags.hpp"
#include "exfs/iterator/legacy.hpp"
#include "exfs/iterator/traits.hpp"
#include "exfs/utility/functions.hpp"

namespace exfs::iterator {
namespace __detail {
template <typename In>
concept __IndirectlyReadableImpl = requires (In const in) {
    typename iter_value_t<In>;
    typename iter_reference_t<In>;
    typename iter_rvalue_reference_t<In>;
    { *in } -> std::same_as<iter_reference_t<In>>;
    { iter_move(in) } -> std::same_as<iter_rvalue_reference_t<In>>;
} and
std::common_reference_with<
    iter_reference_t<In>&&,
    iter_value_t<In>&
> and
std::common_reference_with<
    iter_reference_t<In>&&,
    iter_rvalue_reference_t<In>&&
> and
std::common_reference_with<
    iter_rvalue_reference_t<In>&&,
    iter_value_t<In> const&
>;
}  // __detail

/**
 * The concept @c indirectly_readable is modeled by types that are readable by
 * applying `operator*`, such as pointers, smart pointers, and input iterators.
 */
template <typename In>
concept indirectly_readable =
    __detail::__IndirectlyReadableImpl<std::remove_cvref_t<In>>;

/**
 * The concept `indirectly_writable<Out, T>` specifies the requirements for
 * writing a value whose type and value category are encoded by @p T into the
 * referenced object of an iterator @Out.
 *
 * @tparam Out The iterator type under test.
 * @tparam T The type to write to the dereferenced @p Out instance.
 */
template <typename Out, typename T>
concept indirectly_writable = requires (Out&& out, T&& t) {
    *out = exfs::forward<T>(t);
    *exfs::forward<Out>(out) = exfs::forward<T>(t);

    // These expressions prevent `indirectly_readable` objects with prvalue
    // reference types from satisfying the syntactic requirements of
    // `indirectly_writable` by accident, while permitting proxy references to
    // continue to work as long as their constness is shallow.
    // https://github.com/ericniebler/stl2/issues/381
    const_cast<iter_reference_t<Out> const&&>(*out) = exfs::forward<T>(t);
    const_cast<iter_reference_t<Out> const&&>(*exfs::forward<Out>(out)) =
        exfs::forward<T>(t);
};

/**
 * This concept specifies requirements on types that can be incremented with the
 * pre- and post-increment operators, but those increment operations are not
 * necessarily equality-preserving, and the type itself is not required to
 * be @c equality_comparable.
 *
 * For @c weakly_incrementable types, `a == b` does not imply that `++a == ++b`.
 * Algorithms on weakly incrementable types must be single-pass algorithms.
 * These algorithms can be used with istreams as the source of the input data
 * through @c istream_iterator.
 */
template <typename I>
concept weakly_incrementable =
    std::movable<I> and
    requires (I i) {
        typename iter_difference_t<I>;
        requires std::signed_integral<iter_difference_t<I>>;
        { ++i } -> std::same_as<I&>;  // not required to be equality-preserving
        i++;                          // not required to be equality-preserving
    };

/**
 * This concept specifies requirements on types that can be incremented with the
 * pre- and post-increment operators, whose increment operations are equality-
 * preserving, and the type is @c std::equality_comparable.
 *
 * Unlike with @c weakly_incrementable, which only support single-pass
 * algorithms, multi-pass one-directional algorithms can be used with types
 * that model @c incrementable.
 *
 * @name Semantic Requirements
 *
 * `I` models @c incrementable only if given any two incrementable objects @c a
 * and @c b of type @c I:
 *   - `bool(a == b)` implies bool(a++ == b)
 *   - `bool(a == b)` implies `bool(((void)a++, a) == ++b)
 */
template <typename I>
concept incrementable =
    std::regular<I> and
    weakly_incrementable<I> and
    requires (I i) {
        { i++ } -> std::same_as<I>;
    };

/**
 * The @c input_or_output_iterator concept forms the basis of the iterator
 * concept taxonomy; every iterator type satisfies the @c
 * input_or_output_iterator requirements.
 *
 * `input_or_output_iterator` itself only specifies operations for dereferencing
 * and incrementing an iterator. Most algorithms will require additional
 * operations, for example:
 *   - comparing iterators with sentinels (see @c sentinel_for)
 *   - reading values from an iterator (see @c indirectly_readable and @c
 *     input_iterator)
 *   - writing values to an iterator (see @c indirectly_writable and @c
 *     output_iterator)
 *   - a richer set of iterator movements (see @c forward_iterator, @c
 *     bidirectional_iterator, @c random_access_iterator)
 *
 * Unlike the @c LegacyIterator requirements, the @c input_or_output_iterator
 * concept does not require copyability.
 */
template <typename I>
concept input_or_output_iterator = requires (I i) {
    { *i } -> referenceable;
} and weakly_incrementable<I>;

/**
 * The @c sentinel_for concept specifies the relationship between an @c
 * input_or_output_iterator type and a @c semiregular type whose values denote
 * a range.
 */
template <typename S, typename I>
concept sentinel_for =
    std::semiregular<S> and
    exfs::iterator::input_or_output_iterator<I> and
    exfs::weakly_equality_comparable_with<S, I>;

/**
 * This variable template can be used to prevent iterators and sentinels that
 * can be subtracted but do not actually model @c sized_sentinel_for from
 * satisfying the concept.
 *
 * The variable template is allowed to be specialized for cv-unqualified
 * non-array object types @p S and @p I, as long as at least one of which is a
 * program-defined type. Such specializations shall be usable in constant
 * expressions and have type `const bool`.
 */
template <typename S, typename I>
inline constexpr bool disable_sized_sentinel_for = false;

/**
 * This concept specifies that objects of the specified types can be subtracted
 * to compute the distance between them in constant time.
 *
 * Let @c i be an iterator of type @p I, and @c s be a sentinel of type @p S
 * such that `[i, s)` denotes a range. Let @c n be the smallest number of
 * applications of @c ++i necessary to make `bool(i == s)` be true. @p I and @p
 * S model `sized_sentinel_for<S, I>` only if all the following are true:
 *   - if @c n is representable by @c iter_difference_t<I>, then `s - i` is
 *     well-defined and equals @c n
 *   - if @c -n is representable by @c iter_difference_t<I>, then `i - s` is
 *     well-defined and equals @c -n
 *   - subtraction between @c i and @c s has constant time complexity
 *
 * @tparam S The sentinel type.
 * @tparam I The iterator type @p S is potentially a sentinel for.
 */
template <typename S, typename I>
concept sized_sentinel_for =
    sentinel_for<S, I> and
    not disable_sized_sentinel_for<std::remove_cv_t<S>, std::remove_cv_t<I>> and
    requires (I const& i, S const& s) {
        { s - i } -> std::same_as<iter_difference_t<I>>;
        { i - s } -> std::same_as<iter_difference_t<I>>;
    };

namespace __detail {
template <typename I>
struct __iter_concept_impl;

// ITER_CONCEPT(I) is ITER_TRAITS(I)::iterator_concept if that is valid.
template <typename I>
requires requires { typename iterator_traits<I>::iterator_concept; }
struct __iter_concept_impl<I> {
    using type = typename iterator_traits<I>::iterator_concept;
};

// Else, ITER_TRAITS(I)::iterator_category if that is valid.
template <typename I>
requires (
    not requires { typename iterator_traits<I>::iterator_concept; } and
    requires { typename iterator_traits<I>::iterator_category; }
)
struct __iter_concept_impl<I> {
    using type = typename iterator_traits<I>::iterator_category;
};

// Else, random_access_tag if iterator_traits<I> is not specialized.
template <typename I>
requires (
    not requires { typename iterator_traits<I>::iterator_concept; } and
    not requires { typename iterator_traits<I>::iterator_category; } and
    std::is_base_of_v<__iter_traits<I>, iterator_traits<I>>
)
struct __iter_concept_impl<I> {
    using type = random_access_iterator_tag;
};

// Else, there is no ITER_CONCEPT(I) type.
template <typename I>
struct __iter_concept_impl {};

// ITER_CONCEPT
template <typename I>
using __iter_concept = typename __iter_concept_impl<I>::type;
}  // namespace __detail

/**
 * The @c input_iterator concept is a refinement of @c input_or_output_iterator,
 * adding the requirement that the referenced values can be read
 * (via @c indirectly_readable) and the requirement that the iterator concept
 * tag be present.
 */
template <typename I>
concept input_iterator =
    input_or_output_iterator<I> and
    indirectly_readable<I> and
    requires { typename __detail::__iter_concept<I>; } and
    std::derived_from<__detail::__iter_concept<I>, input_iterator_tag>;

/**
 * The @c output_iterator concept refines @c input_or_output_iterator, adding
 * the requirement that it can be used to write values of type and value
 * category encoded by @p T. Note that @c equality_comparable is not required.
 */
template <typename I, typename T>
concept output_iterator =
    input_or_output_iterator<I> and
    indirectly_writable<I, T> and
    requires (I i, T&& t) {
        *i++ = exfs::forward<T>(t);  // not required to be equality-preserving
    };

/**
 * This concept refines @c input_iterator by requiring that @p I also model @c
 * incrementable (thereby making it suitable for multi-pass algorithms), and
 * guaranteeing that two iterators to the same range can be compared against
 * each other.
 */
template <typename I>
concept forward_iterator =
    input_iterator<I> and
    std::derived_from<__detail::__iter_concept<I>, forward_iterator_tag> and
    incrementable<I> and
    sentinel_for<I, I>;

/**
 * The concept @c bidirectional_iterator refines @c forward_iterator by adding
 * the ability to move an iterator backward.
 */
template <typename I>
concept bidirectional_iterator =
    forward_iterator<I> and
    std::derived_from<
        __detail::__iter_concept<I>,
        bidirectional_iterator_tag
    > and
    requires (I i) {
        { --i } -> std::same_as<I&>;
        { i-- } -> std::same_as<I>;
    };
}  // exfs::iterator

#endif  // EXFS_ITERATOR_CONCEPTS_HPP_

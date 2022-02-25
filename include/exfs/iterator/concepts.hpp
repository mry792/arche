#ifndef EXFS_ITERATOR_CONCEPTS_HPP_
#define EXFS_ITERATOR_CONCEPTS_HPP_

#include <concepts>
#include <type_traits>

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
}  // exfs::iterator

#endif  // EXFS_ITERATOR_CONCEPTS_HPP_

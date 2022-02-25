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
}  // exfs::iterator

#endif  // EXFS_ITERATOR_CONCEPTS_HPP_

#ifndef EXFS_ITERATOR_CONCEPTS_HPP_
#define EXFS_ITERATOR_CONCEPTS_HPP_

#include <concepts>
#include <type_traits>

#include "exfs/iterator/traits.hpp"

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
}  // exfs::iterator

#endif  // EXFS_ITERATOR_CONCEPTS_HPP_

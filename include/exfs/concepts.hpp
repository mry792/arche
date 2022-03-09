#ifndef EXFS_CONCEPTS_HPP_
#define EXFS_CONCEPTS_HPP_

#include <concepts>
#include <type_traits>

#include "exfs/utility/functions.hpp"

namespace exfs {
namespace __detail {
template <typename T>
using __with_ref = T&;
}  // __detail

/**
 * Indicates if @p T can be referenced (in particular, not @c void).
 */
template <typename T>
concept referenceable = requires {
    typename __detail::__with_ref<T>;
};

/**
 * Indicates if @p T can be dereferenced.
 */
template <typename T>
concept dereferenceable = requires (T& t) {
    { *t } -> referenceable;
};

/**
 * The concept @c boolean_testable specifies the requirements for expressions
 * that are convertible to @c bool and for which the logical operators have the
 * usual behavior (including short-circuiting), even for two different @c
 * boolean-testable types.
 *
 * Formally, to model the concept @c boolean_testable, the type must not define
 * any member operators @c && and @c ||, and no viable non-member
 * operators @c && and @c || may be visible by argument-dependent lookup.
 * Additionally, given an expression @c e such that @c decltype((e)) is @c B,
 * @c boolean_testable is modeled only if `bool(e) == !bool(!e)`.
 */
template <typename B>
concept boolean_testable =
    std::convertible_to<B, bool> and
    requires (B&& b) {
        { !exfs::forward<B>(b) } -> std::convertible_to<bool>;
    };

/**
 * The concept @c weakly_equality_comparable_with specifies that an object of
 * type @p T and an object of type @p U can be compared for equality with each
 * other (in either order) using both @c == and @c !=, and the results of the
 * comparisons are consistent.
 *
 *
 */
template <typename T, typename U>
concept weakly_equality_comparable_with = requires(
    std::remove_reference_t<T> const& t,
    std::remove_reference_t<U> const& u
) {
    { t == u } -> boolean_testable;
    { t != u } -> boolean_testable;
    { u == t } -> boolean_testable;
    { u != t } -> boolean_testable;
};
}  // exfs

#endif  // EXFS_CONCEPTS_HPP_

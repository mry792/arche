#ifndef EXFS_ITERATOR_TRAITS_HPP_
#define EXFS_ITERATOR_TRAITS_HPP_

#include <concepts>
#include <cstddef>
#include <type_traits>

#include "exfs/utility/functions.hpp"

namespace exfs::iterator {
/**
 * @name incrementable_traits
 * @{
 */

/**
 * Computes the associated difference type, if any. Users may specialize @c
 * incrementable_traits for a program-defined type.
 *
 * The primary template is an empty struct.
 */
template <typename I>
struct incrementable_traits {};

/**
 * Specialization for pointers. Provides a member type @c difference_type.
 */
template <typename T>
requires (std::is_object_v<T>)
struct incrementable_traits<T*> {
    using difference_type = std::ptrdiff_t;
};

/**
 * Specialization for const-qualified types.
 */
template <typename T>
struct incrementable_traits<T const> : incrementable_traits<T> {};

/**
 * Specialization for types that define a public and accessible member type @c
 * difference_type.
 */
template <typename T>
requires requires { typename T::difference_type; }
struct incrementable_traits<T> {
    using difference_type = typename T::difference_type;
};

/**
 * Specialization for types that do not define a public and accessible member
 * type @c difference_type but do support subtraction.
 */
template <typename T>
requires (not requires { typename T::difference_type; }) and
    requires (T const& a, T const& b) {
        { a - b } -> std::integral;
    }
struct incrementable_traits<T> {
    using difference_type = std::make_signed_t<
        decltype(exfs::declval<T>() - exfs::declval<T>())>;
};

/**
 * @}
 */

/**
 * @name indirectly_readable_traits
 * @{
 */

/**
 * Computes the associated value type of the given type, if any. Users may
 * specialize @c indirectly_readable_traits for a program-defined type.
 *
 * The primary template has no member @c value_type.
 */
template <typename I>
struct indirectly_readable_traits {};

/**
 * Specialization for pointers. Only valid for pointers to object types.
 */
template <typename T>
requires (std::is_object_v<T>)
struct indirectly_readable_traits<T*> {
    using value_type = std::remove_cv_t<T>;
};

/**
 * Specialization for arrays.
 */
template <typename I>
requires (std::is_array_v<I>)
struct indirectly_readable_traits<I> {
    using value_type = std::remove_cv_t<std::remove_extent_t<I>>;
};

/**
 * Specialization to ignore const qualifiers.
 */
template <typename T>
struct indirectly_readable_traits<T const> : indirectly_readable_traits<T> {};

/**
 * Specialization for @c value_type member.
 */
template <typename T>
requires (
    requires { typename T::value_type; } and
    not requires { typename T::element_type; } and
    std::is_object_v<typename T::value_type>
)
struct indirectly_readable_traits<T> {
    using value_type = std::remove_cv_t<typename T::value_type>;
};

/**
 * Specialization for @c element_type member.
 */
template <typename T>
requires (
    not requires { typename T::value_type; } and
    requires { typename T::element_type; } and
    std::is_object_v<typename T::element_type>
)
struct indirectly_readable_traits<T> {
    using value_type = std::remove_cv_t<typename T::element_type>;
};

/**
 * Specialization for having both @c value_type and @c element_type members.
 */
template <typename T>
requires (
    requires {
        typename T::value_type;
        typename T::element_type;
    } and
    std::is_object_v<typename T::value_type> and
    std::is_object_v<typename T::element_type> and
    std::same_as<
        std::remove_cv_t<typename T::value_type>,
        std::remove_cv_t<typename T::element_type>
    >
)
struct indirectly_readable_traits<T> {
    using value_type = std::remove_cv_t<typename T::value_type>;
};

/**
 * @}
 */
}  // namespace exfs::iterator

#endif  // EXFS_ITERATOR_TRAITS_HPP_

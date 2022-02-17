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
}  // namespace exfs::iterator

#endif  // EXFS_ITERATOR_TRAITS_HPP_

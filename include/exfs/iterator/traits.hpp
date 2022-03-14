#ifndef EXFS_ITERATOR_TRAITS_HPP_
#define EXFS_ITERATOR_TRAITS_HPP_

#include <concepts>
#include <cstddef>
#include <type_traits>

#include "exfs/concepts.hpp"
#include "exfs/iterator/iter_move.hpp"
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

/**
 * @name aliases
 * @{
 */

/**
 * Computes the value type of @p T.
 *
 * @todo Add alternative implementation for specializations of @c
 * iterator_traits.
 *
 * @tparam T The iterator type in question.
 */
template <typename T>
using iter_value_t = typename indirectly_readable_traits<
    std::remove_cvref_t<T>>::value_type;

/**
 * Computes the reference type of @p T.
 *
 * The alias template @c iter_reference_t<T> is equal to the following:
 *   - if @c T::reference exists and names a type, then @c T::reference
 *     (note that this is a deviation from the standard library specification)
 *   - else if T is dereferenceable, this is @c decltype(*declval<T&>())
 *   - else @c iter_reference_t is undefined
 *
 * @tparam T The iterator type in question.
 */
template <typename T>
struct iter_reference {};

template <typename T>
requires (requires { typename T::reference; })
struct iter_reference<T> {
    using type = typename T::reference;
};

template <dereferenceable T>
requires (not requires { typename T::reference; })
struct iter_reference<T> {
    using type = decltype(*exfs::declval<T&>());
};

template <typename T>
requires (requires { typename iter_reference<T>::type; })
using iter_reference_t = typename iter_reference<T>::type;

/**
 * Computes the pointer type of @p T.
 *
 * This is not part of the official C++ standard library but something like it
 * is used in the implementation of @c iterator_traits in some other
 * implementations.
 *
 * The alias template @c iter_pointer_t<T> is equal to the following:
 *   - if @p T is a pointer, then @p T
 *   - else if @c T::pointer exists and names a type, then @c T::pointer
 *   - else if @c t.operator->() is defined, then it @c iter_pointer_t<T> is the
 *     result type of that operator
 *   - else @c iter_pointer_t<T> is undefined
 *
 * @note The trait alias @c iter_pointer_t is transparent to const/volatile and
 *     reference qualifiers.
 *
 * @tparam T The iterator type in question
 */
template <typename T>
struct iter_pointer {};

template <typename T>
struct iter_pointer<T*> {
    using type = T*;
};

template <typename T>
requires (requires { typename T::pointer; })
struct iter_pointer<T> {
    using type = typename T::pointer;
};

template <typename T>
requires (
    not requires { typename T::pointer; } and
    requires {
        exfs::declval<T&>().operator->();
    }
)
struct iter_pointer<T> {
    using type = decltype(exfs::declval<T&>().operator->());
};

template <typename T>
using iter_pointer_t = typename iter_pointer<std::remove_cvref_t<T>>::type;

/**
 * Computes the difference type of @p T.
 *
 * @todo Add alternative implementation for specializations of @c
 * iterator_traits.
 *
 * @tparam T The iterator type in question.
 */
template <typename T>
using iter_difference_t = typename incrementable_traits<
    std::remove_cvref_t<T>>::difference_type;

template <dereferenceable T>
using iter_rvalue_reference_t =
    decltype(iter_move(exfs::declval<T&>()));

template <typename T>
using iter_common_reference_t = std::common_reference_t<
    iter_reference_t<T>,
    iter_value_t<T>&
>;

/**
 * @}
 */
}  // namespace exfs::iterator

#endif  // EXFS_ITERATOR_TRAITS_HPP_

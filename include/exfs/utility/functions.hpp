#ifndef EXFS_UTILITY_FUNCTIONS_HPP_
#define EXFS_UTILITY_FUNCTIONS_HPP_

#include <cstddef>

#include <concepts>
#include <type_traits>

namespace exfs {
// TODO: forward()

/*!
    Used to *indicate* that an object may be "moved from," i.e. allowing the
    efficient transfer of resources from @p t to another object.

    @param[in] t The object to be moved.
 */
template <typename T>
constexpr std::remove_reference_t<T>&& move (T&& t) noexcept {
    return static_cast<std::remove_reference_t<T>&&>(t);
}

// TODO: move_if_noexcept()

/*!
    Forms lvalue reference to const type of @p t.
 */
template <typename T>
constexpr std::add_const_t<T>& as_const(T& t) noexcept {
    return t;
}

template <typename T>
void as_const (T const&&) = delete;

// TODO: declval()

/*!
    Exchanges the given values.

    @param[in,out] a
    @param[in,out] b
 */
template <typename T>
requires std::move_constructible<T> and std::is_move_assignable_v<T>
constexpr void swap (T& a, T& b) noexcept(
    std::is_nothrow_move_constructible_v<T> and
    std::is_nothrow_move_assignable_v<T>
) {
    T tmp = move(a);
    a = move(b);
    b = move(tmp);
}

// TODO: swap() for built-in arrays

// TODO: exchange()
}  // namespace exfs

#endif  // EXFS_UTILITY_FUNCTIONS_HPP_

#ifndef EXFS_UTILITY_PAIR_HPP_
#define EXFS_UTILITY_PAIR_HPP_

#include <concepts>
#include <type_traits>

#include "exfs/utility/functions.hpp"

namespace exfs::utility {
/*!
    A @c pair is a class template that provides a way to store two heterogeneous
    objects as a single unit. A pair is a specific case of a @c tuple with two
    elements.
 */
template <typename T1, typename T2>
struct pair {
    using first_type = T1;
    using second_type = T2;

    /*!
        Data stored by this @c pair type.
     */
    first_type first{};
    second_type second{};

    /**
     * Default constructor. Value-initializes both elements of the pair, @c
     * first and @c second.
     */
    constexpr pair () {}

    /**
     * Copy constructor is defaulted and is @c constexpr if copying of both
     * elements satisfies the requirements on constexpr functions.
     */
    constexpr pair (pair const&) = default;

    /**
     * Move constructor is defaulted and is @c constexpr if moving of both
     * elements satisfies the requirements on constexpr functions.
     */
    constexpr pair (pair&&) = default;

    /**
     * Initializes @c first with @p x and @c second with @p y. The arguments are
     * forwarded to constructors for each element.
     */
    template <typename U1 = T1, typename U2 = T2>
    constexpr pair (U1&& x, U2&& y)
      : first{exfs::forward<U1>(x)},
        second{exfs::forward<U2>(y)}
    {}

    /**
     * Initialize @c first with @p other.first and @c second with @p
     * other.second. This constructor participates in overload resolution if @c
     * other is not the same type as the constructed object.
     */
    template <typename U1, typename U2>
    requires (not std::same_as<pair<U1, T2>, pair>)
    constexpr pair (pair<U1, U2> const& other)
      : first{other.first},
        second{other.second}
    {}

    /**
     * Initialize @c first with @p other.first and @c second with @p
     * other.second. The arguments are forwarded to constructors for each
     * element. This constructor participates in overload resolution if @c
     * other is not the same type as the constructed object.
     */
    template <typename U1, typename U2>
    requires (not std::same_as<pair<U1, T2>, pair>)
    constexpr pair (pair<U1, U2>&& other)
      : first{exfs::forward<U1>(other.first)},
        second{exfs::forward<U2>(other.second)}
    {}
};
}  // namespace exfs::utility

#endif  // EXFS_UTILITY_PAIR_HPP_

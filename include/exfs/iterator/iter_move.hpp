#ifndef EXFS_ITERATOR_ITER_MOVE_HPP_
#define EXFS_ITERATOR_ITER_MOVE_HPP_

#include <type_traits>

#include "exfs/dereferenceable.hpp"
#include "exfs/utility/functions.hpp"

namespace exfs::iterator {
namespace __cust_iter_move {
// Base case for ADL. Should never actually be called.
void iter_move ();

template <typename T>
concept __class_or_enum =
    std::is_class_v<T> or
    std::is_union_v<T> or
    std::is_enum_v<T>;

template <typename T>
concept __adl_iter_move = __class_or_enum<std::remove_reference_t<T>> and
    requires(T&& t) {
        iter_move(static_cast<T&&>(t));
    };

struct __iter_move_fn {
  private:
    template <typename T>
    using __iter_reference_t = decltype(*declval<T&>());

    template <typename T>
    struct __result {
        using type = __iter_reference_t<T>;
    };

    template <typename T>
    requires __adl_iter_move<T>
    struct __result<T> {
        using type = decltype(iter_move(declval<T>()));
    };

    template <typename T>
    requires (!__adl_iter_move<T>) and
        std::is_lvalue_reference_v<__iter_reference_t<T>>
    struct __result<T> {
        using type = std::remove_reference_t<__iter_reference_t<T>>&&;
    };

    template <typename T>
    static constexpr bool __is_noexcept () {
        if constexpr (__adl_iter_move<T>) {
            return noexcept(iter_move(declval<T>()));
        } else {
            return noexcept(*declval<T>());
        }
    }

  public:
    // The result type of iter_move(std::declval<T>())
    template <dereferenceable T>
    using __type = typename __result<T>::type;

    template <dereferenceable T>
    constexpr __type<T> operator () (T&& t) const
        noexcept(__is_noexcept<T>())
    {
        if constexpr (__adl_iter_move<T>) {
            return iter_move(static_cast<T&&>(t));
        } else if constexpr (
            std::is_lvalue_reference_v<__iter_reference_t<T>>
        ) {
            return static_cast<__type<T>>(*t);
        } else {
            return *t;
        }
    }
};
}  // namespace __cust_iter_move

inline namespace __cust {
/**
 * Obtains an rvalue reference or a prvalue temporary from a given iterator.
 *
 * This is a customization point object. It provides a default implementation
 * but alternatives can be provided that are found by ADL.
 */
inline constexpr __cust_iter_move::__iter_move_fn iter_move{};
}  // inline namespace __cust
}  // namespace exfs::iterator

#endif  // EXFS_ITERATOR_ITER_MOVE_HPP_

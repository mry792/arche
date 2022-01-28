#include "exfs/utility/functions.hpp"

#include <string>
#include <utility>

#include <catch2/catch.hpp>

#define MAKE_WRAPPER(WRAPPER_NAME, WRAPPED_FUNC)                               \
namespace {                                                                    \
struct WRAPPER_NAME {                                                          \
    template <typename... Ts>                                                  \
    constexpr decltype(auto) operator () (Ts&&... ts) const                    \
    noexcept(noexcept(WRAPPED_FUNC(std::forward<Ts>(ts)...))) {                \
        return WRAPPED_FUNC(std::forward<Ts>(ts)...);                          \
    }                                                                          \
};                                                                             \
}

///
/// exfs::move()
///

MAKE_WRAPPER(std_move, std::move)
MAKE_WRAPPER(exfs_move, exfs::move)

TEMPLATE_TEST_CASE(
    "exfs::move",
    "[unit][std-parity][utility]",
    std_move,
    exfs_move
) {
    TestType wrapper{};

    GIVEN("some variables") {
        int i = -7;
        double d = 4.13;
        std::string s = "asdf";
        double& d_ref = d;
        std::string const& s_cref = s;

        THEN("X::move() is noexcept") {
            CHECK(noexcept(wrapper(i)));
            CHECK(noexcept(wrapper(d)));
            CHECK(noexcept(wrapper(s)));
            CHECK(noexcept(wrapper(d_ref)));
            CHECK(noexcept(wrapper(s_cref)));
        }

        THEN("X::move() returns the same type but as r-value reference") {
            CHECK(std::is_same_v<decltype(wrapper(i)), int&&>);
            CHECK(std::is_same_v<decltype(wrapper(d)), double&&>);
            CHECK(std::is_same_v<decltype(wrapper(s)), std::string&&>);
            CHECK(std::is_same_v<decltype(wrapper(d_ref)), double&&>);
            CHECK(std::is_same_v<decltype(wrapper(s_cref)), std::string const&&>);
        }
    }
}

///
/// exfs::forward()
///

namespace {
int get_prvalue () { return 0; }
}

TEST_CASE(
    "exfs::forward",
    "[unit][std-parity][utility]"
) {
    #define CHECK_EXPR_TYPE(EXPR, TYPE) \
    CHECK(std::is_same_v<decltype(EXPR), TYPE>)

    GIVEN("an input type of an lvalue") {
        int const i = -6.5;
        std::string sc = "qwerty";

        THEN("the type is forwarded appropriately") {
            #define DO_CHECK(FUNC)                                             \
            CHECK_EXPR_TYPE(FUNC<int const>(i), int const&&);                  \
            CHECK_EXPR_TYPE(FUNC<int const&>(i), int const&);                  \
            CHECK_EXPR_TYPE(FUNC<int const&&>(i), int const&&);                \
            CHECK_EXPR_TYPE(FUNC<std::string>(sc), std::string&&);             \
            CHECK_EXPR_TYPE(FUNC<std::string&>(sc), std::string&);             \
            CHECK_EXPR_TYPE(FUNC<std::string&&>(sc), std::string&&);

            DO_CHECK(std::forward)
            DO_CHECK(exfs::forward)

            #undef DO_CHECK
        }
    }

    GIVEN("an input type of a prvalue") {
        THEN("the type is forwarded appropriately") {
            #define DO_CHECK(FUNC)                                             \
            CHECK_EXPR_TYPE(FUNC<int>(get_prvalue()), int&&);                  \
            CHECK_EXPR_TYPE(FUNC<int&>(get_prvalue()), int&);                  \
            CHECK_EXPR_TYPE(FUNC<int&&>(get_prvalue()), int&&);

            DO_CHECK(std::forward)
            DO_CHECK(exfs::forward)

            #undef DO_CHECK
        }
    }

    GIVEN("an input type of an xvalue") {
        int i = 7;

        THEN("the type is forwarded appropriatey") {
            #define DO_CHECK(FUNC)                                             \
            CHECK_EXPR_TYPE(FUNC<int>(std::move(i)), int&&);                   \
            CHECK_EXPR_TYPE(FUNC<int&>(std::move(i)), int&);                   \
            CHECK_EXPR_TYPE(FUNC<int&&>(std::move(i)), int&&);

            DO_CHECK(std::forward);
            DO_CHECK(exfs::forward);

            #undef DO_CHECK
        }
    }

    #undef CHECK_EXPR_TYPE
}

///
/// exfs::as_const()
///

MAKE_WRAPPER(std_as_const, std::as_const)
MAKE_WRAPPER(exfs_as_const, exfs::as_const)

TEMPLATE_TEST_CASE(
    "exfs::as_const",
    "[unit][std-parity][utility]",
    std_as_const,
    exfs_as_const
) {
    TestType wrapper{};

    GIVEN("some objects") {
        int i = -7;
        double d = 4.13;
        std::string s = "asdf";

        THEN("X::as_const() is noexcept") {
            CHECK(noexcept(wrapper(i)));
            CHECK(noexcept(wrapper(d)));
            CHECK(noexcept(wrapper(s)));
        }

        THEN("X::as_const() returns the same type but const") {
            CHECK(std::is_same_v<decltype(wrapper(i)), int const&>);
            CHECK(std::is_same_v<decltype(wrapper(d)), double const&>);
            CHECK(std::is_same_v<decltype(wrapper(s)), std::string const&>);
        }
    }

    GIVEN("some const objects") {
        int const i = -7;
        double const d = 4.13;
        std::string const s = "asdf";

        THEN("X::as_const() is noexcept") {
            CHECK(noexcept(wrapper(i)));
            CHECK(noexcept(wrapper(d)));
            CHECK(noexcept(wrapper(s)));
        }

        THEN("X::as_const() returns the same type but const") {
            CHECK(std::is_same_v<decltype(wrapper(i)), int const&>);
            CHECK(std::is_same_v<decltype(wrapper(d)), double const&>);
            CHECK(std::is_same_v<decltype(wrapper(s)), std::string const&>);
        }
    }
}

///
/// exfs::swap()
///

MAKE_WRAPPER(std_swap, std::swap)
MAKE_WRAPPER(exfs_swap, exfs::swap)

namespace {
template <bool noexcept_move_construct, bool noexcept_assign>
struct Swap_Obj {
    int v;

    Swap_Obj (int v_) : v{v_} {}

    Swap_Obj (Swap_Obj&& other) noexcept(noexcept_move_construct)
          : v{std::move(other.v)} {};

    Swap_Obj& operator = (Swap_Obj&& other) noexcept(noexcept_assign) {
        v = exfs::move(other.v);
        return *this;
    }
};
}  // namespace

TEMPLATE_TEST_CASE(
    "exfs::swap",
    "[unit][std-parity][utility]",
    std_swap,
    exfs_swap
) {
    TestType wrapper{};

    GIVEN("a built-in type") {
        double obj_a = 3.7;
        double obj_b = -4.9;

        THEN("X::swap is noexcept") {
            CHECK(noexcept(wrapper(obj_a, obj_b)));
        }

        THEN("X::swap swaps the values") {
            wrapper(obj_a, obj_b);
            CHECK(obj_a == -4.9);
            CHECK(obj_b == 3.7);
        }
    }

    GIVEN("a nothrow move constructible/assignable class type") {
        using Obj = Swap_Obj<true, true>;

        Obj obj_a{-3};
        Obj obj_b{11};

        THEN("X::swap is noexcept") {
            CHECK(noexcept(wrapper(obj_a, obj_b)));
        }

        THEN("X::swap swaps the values") {
            wrapper(obj_a, obj_b);
            CHECK(obj_a.v == 11);
            CHECK(obj_b.v == -3);
        }
    }

    GIVEN("a class type with a move constructor that could throw") {
        using Obj = Swap_Obj<false, true>;

        Obj obj_a{-3};
        Obj obj_b{11};

        THEN("X::swap is noexcept") {
            CHECK(not noexcept(wrapper(obj_a, obj_b)));
        }

        THEN("X::swap swaps the values") {
            wrapper(obj_a, obj_b);
            CHECK(obj_a.v == 11);
            CHECK(obj_b.v == -3);
        }
    }

    GIVEN("a class type with a move assignment operator that could throw") {
        using Obj = Swap_Obj<true, false>;

        Obj obj_a{-3};
        Obj obj_b{11};

        THEN("X::swap is noexcept") {
            CHECK(not noexcept(wrapper(obj_a, obj_b)));
        }

        THEN("X::swap swaps the values") {
            wrapper(obj_a, obj_b);
            CHECK(obj_a.v == 11);
            CHECK(obj_b.v == -3);
        }
    }
}

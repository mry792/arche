#include "exfs/utility/functions.hpp"

#include <string>
#include <utility>

#include <catch2/catch.hpp>

#define MAKE_WRAPPER(WRAPPER_NAME, WRAPPED_FUNC)                               \
namespace {                                                                    \
struct WRAPPER_NAME {                                                          \
    template <typename... Ts>                                                  \
    constexpr auto&& operator () (Ts&&... ts) const                            \
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
    "exfs::move - same as std::move",
    "[unit][parity][utility]",
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
/// exfs::as_const()
///

MAKE_WRAPPER(std_as_const, std::as_const)
MAKE_WRAPPER(exfs_as_const, exfs::as_const)

TEMPLATE_TEST_CASE(
    "exfs::as_const - same as std::as_const",
    "[unit][parity][utility]",
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

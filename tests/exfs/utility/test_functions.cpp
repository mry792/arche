#include "exfs/utility/functions.hpp"

#include <string>
#include <utility>

#include <catch2/catch.hpp>

#define MAKE_WRAPPER(WRAPPER, WRAPPED)                                         \
struct WRAPPER {                                                               \
    template <typename... Ts>                                                  \
    constexpr auto&& operator () (Ts&&... ts) const                            \
    noexcept(noexcept(WRAPPED(std::forward<Ts>(ts)...))) {                     \
        return WRAPPED(std::forward<Ts>(ts)...);                               \
    }                                                                          \
};

namespace {
MAKE_WRAPPER(std_as_const, std::as_const)
MAKE_WRAPPER(exfs_as_const, exfs::as_const)
}

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

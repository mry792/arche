#include "arche/int_literals.hpp"

#include <cstdint>

#include <type_traits>

#include <catch2/catch.hpp>

TEST_CASE("arche::int_literals", "[unit][int_literals]") {
    using namespace arche::int_literals;

    SECTION("int8_t") {
        REQUIRE(std::is_same_v<decltype(3_i8), std::int8_t>);
        REQUIRE(3_i8 == 3);
    }
    SECTION("int16_t") {
        REQUIRE(std::is_same_v<decltype(3_i16), std::int16_t>);
        REQUIRE(3_i16 == 3);
    }
    SECTION("int32_t") {
        REQUIRE(std::is_same_v<decltype(3_i32), std::int32_t>);
        REQUIRE(3_i32 == 3);
    }
    SECTION("int64_t") {
        REQUIRE(std::is_same_v<decltype(3_i64), std::int64_t>);
        REQUIRE(3_i64 == 3);
    }

    SECTION("uint8_t") {
        REQUIRE(std::is_same_v<decltype(3_u8), std::uint8_t>);
        REQUIRE(3_u8 == 3);
    }
    SECTION("uint16_t") {
        REQUIRE(std::is_same_v<decltype(3_u16), std::uint16_t>);
        REQUIRE(3_u16 == 3);
    }
    SECTION("uint32_t") {
        REQUIRE(std::is_same_v<decltype(3_u32), std::uint32_t>);
        REQUIRE(3_u32 == 3);
    }
    SECTION("uint64_t") {
        REQUIRE(std::is_same_v<decltype(3_u64), std::uint64_t>);
        REQUIRE(3_u64 == 3);
    }
}

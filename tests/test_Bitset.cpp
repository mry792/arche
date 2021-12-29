#include "arche/Bitset.hpp"

#include <cstdint>

#include <tuple>
#include <type_traits>

#include <catch2/catch.hpp>

using Bitset_Types = std::tuple<
    arche::Bitset<7u, std::uint8_t>,
    arche::Bitset<8u, std::uint8_t>,
    arche::Bitset<7u, std::uint16_t>,
    arche::Bitset<16u, std::uint16_t>,
    arche::Bitset<7u, std::uint32_t>,
    arche::Bitset<16u, std::uint32_t>,
    arche::Bitset<29u, std::uint32_t>,
    arche::Bitset<32u, std::uint32_t>
>;

TEST_CASE(
    "arche::Bitset - member aliases and constants",
    "[unit][Bitset][constants]"
) {
    #define TEST_BITSET_STATICS(BIT_COUNT, UNDERLYING) \
    GIVEN("concrete Bitset type") { \
        using Bitset_Type = arche::Bitset<BIT_COUNT, UNDERLYING>; \
        THEN("the type has a member alias `Underlying`") { \
            REQUIRE(std::is_same_v< \
                typename Bitset_Type::Underlying, \
                UNDERLYING \
            >); \
        } \
        THEN("the type has a member constant `bit_count`") { \
            REQUIRE(Bitset_Type::bit_count == BIT_COUNT); \
        } \
    }

    TEST_BITSET_STATICS(7u, std::uint8_t);
    TEST_BITSET_STATICS(8u, std::uint8_t);
    TEST_BITSET_STATICS(7u, std::uint16_t);
    TEST_BITSET_STATICS(16u, std::uint16_t);
    TEST_BITSET_STATICS(7u, std::uint32_t);
    TEST_BITSET_STATICS(16u, std::uint32_t);
    TEST_BITSET_STATICS(29u, std::uint32_t);
    TEST_BITSET_STATICS(32u, std::uint32_t);

    #undef TEST_BITSET_STATICS
}

TEMPLATE_LIST_TEST_CASE(
    "arche::Bitset - default constructor",
    "[unit][Bitset][constructor]",
    Bitset_Types
) {
    GIVEN("a concretet Bitset type") {
        WHEN("default constructing an object") {
            constexpr TestType test_obj{};

            THEN("the object is zero initialized") {
                REQUIRE(test_obj.value() == 0u);
            }
        }
    }
}

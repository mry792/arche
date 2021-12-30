#include "arche/Bitset.hpp"

#include <cstdint>

#include <tuple>

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

#include "arche/Register.hpp"

#include <cstdint>

#include <type_traits>

#include <catch2/catch.hpp>

#include "arche/testing/Mock_Register.hpp"

namespace {
auto& mock_register_value = arche::testing::mock_register_value;
using Mock_Register = arche::testing::Mock_Register;
}  // namespace

TEST_CASE(
    "arche::Register - member aliases and constants",
    "[unit][Register][constants]"
) {
    GIVEN("a concrete Register") {
        THEN("the type has a member alias for the underlying value") {
            CHECK(std::is_same_v<
                typename Mock_Register::Underlying,
                std::uint16_t
            >);
        }

        THEN("the type has a member alias for the associated bitset type") {
            CHECK(std::is_same_v<
                typename Mock_Register::Bitset,
                arche::Bitset<16u, typename Mock_Register::Underlying>
            >);
        }
    }
}

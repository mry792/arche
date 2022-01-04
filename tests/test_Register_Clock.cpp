#include "arche/Register_Clock.hpp"

#include <cstdint>

#include <catch2/catch.hpp>

#include "arche/testing/Mock_Register.hpp"

namespace {
auto& mock_register_value = arche::testing::mock_register_value;
using Mock_Register = arche::testing::Mock_Register;

using Register_Clock = arche::Register_Clock<Mock_Register, std::uint32_t>;
}  // namespace

TEST_CASE(
    "arche::Register_Clock - member aliases and constants",
    "[unit][Register_Clock][constants]"
) {
    GIVEN("a concrete Register_Clock type") {
        THEN("the type has a member alias for the underlying register") {
            CHECK(std::is_same_v<
                typename Register_Clock::Register,
                Mock_Register
            >);
        }

        THEN("the type has a member alias for the rep type") {
            CHECK(std::is_same_v<
                typename Register_Clock::Rep,
                std::uint32_t
            >);
        }
    }
}

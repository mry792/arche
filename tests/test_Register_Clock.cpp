#include "arche/Register_Clock.hpp"

#include <cstdint>

#include <catch2/catch.hpp>

#include "arche/testing/Mock_Register.hpp"

namespace {
auto& mock_register_value = arche::testing::mock_register_value;
using Mock_Register = arche::testing::Mock_Register;

using Register_Clock = arche::Register_Clock<Mock_Register, std::uint32_t>;
}  // namespace

SCENARIO (
    "arche::Register_Clock - member aliases and constants",
    "[unit][Register_Clock][constants]"
) {
    GIVEN ("a concrete Register_Clock type") {
        THEN ("the type has a member alias for the underlying register") {
            CHECK(std::is_same_v<
                typename Register_Clock::Register,
                Mock_Register
            >);
        }

        THEN ("the type has a member alias for the rep type") {
            CHECK(std::is_same_v<
                typename Register_Clock::Rep,
                std::uint32_t
            >);
        }
    }
}

SCENARIO (
    "arche::Register_Clock - now() static function",
    "[unit][Register_Clock]"
) {
    mock_register_value = 0u;

    GIVEN ("a default constructed Register_Clock") {
        Register_Clock clock{};

        THEN ("clock.now() is zero") {
            CHECK(clock.now() == 0u);
        }

        WHEN ("the register is set to a value") {
            mock_register_value = GENERATE(1u, 7u, 187u, 255u);

            THEN ("clock.now() equals the register value") {
                CHECK(clock.now() == mock_register_value);
            }
        }
    }
}

SCENARIO (
    "arche::Register_Clock - on_register_overflow() static function",
    "[unit][Register_Clock]"
) {
    mock_register_value = 0b0011'0101u;

    GIVEN ("a default constructed Register_Clock") {
        Register_Clock clock{};

        WHEN ("the register overflows once") {
            clock.on_register_overflow();

            THEN ("the clock's value is one overflow plus the register value") {
                CHECK(clock.now() == 0b0001'0000'0000'0011'0101u);
            }
        }

        WHEN ("the register overflows several times") {
            clock.on_register_overflow();
            clock.on_register_overflow();
            clock.on_register_overflow();

            THEN ("the clock's value is that many overflows plus the register value") {
                CHECK(clock.now() == 0b0011'0000'0000'0011'0101u);
            }
        }
    }
}

#include "arche/Register.hpp"

#include <cstdint>

#include <type_traits>

#include <catch2/catch.hpp>

#include "arche/int_literals.hpp"
#include "arche/testing/Mock_Register.hpp"

namespace {
auto& mock_register_value = arche::testing::mock_register_value;
using Mock_Register = arche::testing::Mock_Register;
}  // namespace

SCENARIO (
    "arche::Register - member aliases and constants",
    "[unit][Register][constants]"
) {
    GIVEN ("a concrete Register") {
        THEN ("the type has a member alias for the underlying value") {
            CHECK(std::is_same_v<
                typename Mock_Register::Underlying,
                std::uint16_t
            >);
        }

        THEN ("the type has a member alias for the associated bitset type") {
            CHECK(std::is_same_v<
                typename Mock_Register::Bitset,
                arche::Bitset<16u, typename Mock_Register::Underlying>
            >);
        }
    }
}

SCENARIO (
    "arche::Register - set() static function",
    "[unit][Register]"
) {
    using Bitset = Mock_Register::Bitset;

    GIVEN ("a Register with a non-zero initial value") {
        constexpr std::uint16_t initial_value = 0b0000'1010'0011'1110;
        mock_register_value = initial_value;

        WHEN ("one sets it to a different value") {
            constexpr std::uint16_t new_value = 0b1101'0000'1111'0110;
            Mock_Register::set(Bitset{new_value});

            THEN ("the register has the new value") {
                CHECK(mock_register_value == new_value);
                CHECK(Mock_Register::get().value() == new_value);
            }
        }
    }
}

SCENARIO (
    "arche::Register - set_bits() static function",
    "[unit][Register]"
) {
    using namespace arche::int_literals;
    using Bitset = Mock_Register::Bitset;

    GIVEN ("a Register with value `0110 1010`") {
        constexpr std::uint16_t initial_value = 0b1001'0101'0110'1010;
        mock_register_value = initial_value;

        WHEN ("one sets all bits") {
            Mock_Register::set_bits(Bitset{Bitset::used_bits_mask()});

            THEN ("all bits are set") {
                CHECK(mock_register_value == 0b1111'1111'1111'1111);
            }
        }

        WHEN ("one sets no bits") {
            Mock_Register::set_bits(Bitset{});

            THEN ("the register value is unchanged") {
                CHECK(mock_register_value == initial_value);
            }
        }

        WHEN ("one sets an already set bit") {
            Mock_Register::set_bits(Mock_Register::Bitset::bit<1>());

            THEN ("the register value is unchanged") {
                CHECK(mock_register_value == initial_value);
            }
        }

        WHEN ("one sets a cleared bit") {
            constexpr unsigned bit_index = 2u;
            Mock_Register::set_bits(Mock_Register::Bitset::bit<bit_index>());

            THEN ("the register value includes that bit set") {
                CHECK(mock_register_value != initial_value);

                const std::uint16_t expected_value =
                    mock_register_value | (1_u16 << bit_index);
                CHECK(mock_register_value == expected_value);
            }
        }
    }
}

SCENARIO (
    "arche::Register - clear_bits() static function",
    "[unit][Register]"
) {
    using namespace arche::int_literals;
    using Bitset = Mock_Register::Bitset;

    GIVEN ("a Register with value `0110 1010`") {
        constexpr std::uint16_t initial_value = 0b1001'0101'0110'1010;
        mock_register_value = initial_value;

        WHEN ("one clears all bits") {
            Mock_Register::clear_bits(Bitset{Bitset::used_bits_mask()});

            THEN ("all bits are set") {
                CHECK(mock_register_value == 0u);
            }
        }

        WHEN ("one clears no bits") {
            Mock_Register::clear_bits(Bitset{});

            THEN ("the register value is unchanged") {
                CHECK(mock_register_value == initial_value);
            }
        }

        WHEN ("one clears an already cleared bit") {
            Mock_Register::clear_bits(Mock_Register::Bitset::bit<2>());

            THEN ("the register value is unchanged") {
                CHECK(mock_register_value == initial_value);
            }
        }

        WHEN ("one clears a set bit") {
            constexpr unsigned bit_index = 3u;
            Mock_Register::clear_bits(Mock_Register::Bitset::bit<bit_index>());

            THEN ("the register value includes that bit as cleared") {
                CHECK(mock_register_value != initial_value);

                const std::uint16_t expected_value =
                    mock_register_value & ~(1_u16 << bit_index);
                CHECK(mock_register_value == expected_value);
            }
        }
    }
}

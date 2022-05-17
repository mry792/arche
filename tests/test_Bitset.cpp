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

TEST_CASE (
    "arche::Bitset - member aliases and constants",
    "[unit][Bitset][constants]"
) {
    #define TEST_BITSET_STATICS(BIT_COUNT, UNDERLYING) \
    GIVEN ("concrete Bitset type") { \
        using Bitset_Type = arche::Bitset<BIT_COUNT, UNDERLYING>; \
        THEN ("the type has a member alias `Underlying`") { \
            REQUIRE(std::is_same_v< \
                typename Bitset_Type::Underlying, \
                UNDERLYING \
            >); \
        } \
        THEN ("the type has a member constant `bit_count`") { \
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

TEST_CASE (
    "arche::Bitset - used_bits_mask() static function",
    "[unit][Bitset]"
) {
    using arche::Bitset;

    REQUIRE(Bitset<7u, std::uint8_t>::used_bits_mask() == 0x7f);
    REQUIRE(Bitset<8u, std::uint8_t>::used_bits_mask() == 0xff);

    REQUIRE(Bitset<7u, std::uint16_t>::used_bits_mask() == 0x00'7f);
    REQUIRE(Bitset<16u, std::uint16_t>::used_bits_mask() == 0xff'ff);

    REQUIRE(Bitset<7u, std::uint32_t>::used_bits_mask() == 0x00'00'00'7f);
    REQUIRE(Bitset<16u, std::uint32_t>::used_bits_mask() == 0x00'00'ff'ff);
    REQUIRE(Bitset<29u, std::uint32_t>::used_bits_mask() == 0x1f'ff'ff'ff);
    REQUIRE(Bitset<32u, std::uint32_t>::used_bits_mask() == 0xff'ff'ff'ff);
}

TEMPLATE_LIST_TEST_CASE (
    "arche::Bitset - default constructor",
    "[unit][Bitset][constructor]",
    Bitset_Types
) {
    GIVEN ("a concretet Bitset type") {
        WHEN ("default constructing an object") {
            constexpr TestType test_obj{};

            THEN ("the object is zero initialized") {
                REQUIRE(test_obj.value() == 0u);
            }
        }
    }
}

TEMPLATE_LIST_TEST_CASE (
    "arche::Bitset - copy constructor",
    "[unit][Bitset][constructor]",
    Bitset_Types
) {
    GIVEN ("a non-default constructed Bitset") {
        using Underlying = typename TestType::Underlying;

        const TestType src{static_cast<Underlying>(0x4bu)};

        WHEN ("a new Bitset is copy-constructed") {
            const TestType test_obj{src};

            THEN ("it has the value of the original object") {
                REQUIRE(test_obj.value() == src.value());
            }
        }
    }
}

TEMPLATE_LIST_TEST_CASE (
    "arche::Bitset - copy assignment operator",
    "[unit][Bitset]",
    Bitset_Types
) {
    GIVEN ("two non-default constructed Bitsets of different values") {
        using Underlying = typename TestType::Underlying;

        const TestType src{static_cast<Underlying>(0x4bu)};
        TestType test_obj{static_cast<Underlying>(0x73u)};
        REQUIRE(src.value() != test_obj.value());

        WHEN ("one Bitset is copy-assigned from the other") {
            test_obj = src;

            THEN ("it has the value of the original object") {
                REQUIRE(test_obj.value() == src.value());
            }
        }
    }
}

TEMPLATE_LIST_TEST_CASE (
    "arche::Bitset - value constructor",
    "[unit][Bitset][constructor]",
    Bitset_Types
) {
    using Underlying = typename TestType::Underlying;

    GIVEN ("an instance of the underlying value") {
        constexpr Underlying init = 0b0001'0110;

        WHEN ("constucting an object with the underlying value") {
            constexpr TestType test_obj{init};

            THEN ("the object takes the value specified.") {
                REQUIRE(test_obj.value() == init);
            }
        }
    }

    if constexpr (not TestType::is_saturated) {
        GIVEN ("an instance of the underlying value with extra bits set") {
            constexpr auto bit_count = TestType::bit_count;
            constexpr Underlying valid_value = 0b0110;
            constexpr Underlying extra_bits = 1 << bit_count;
            constexpr Underlying init = valid_value | extra_bits;

            WHEN ("constructing an object with the underlying value") {
                constexpr TestType test_obj{init};

                THEN ("the object takes the value of the in-range bits") {
                    REQUIRE(test_obj.value() == valid_value);
                }
            }
        }
    }
}

TEMPLATE_LIST_TEST_CASE (
    "arche::Bitset - bit() static function (named constructor)",
    "[unit][Bitset]",
    Bitset_Types
) {
    GIVEN ("a concrete Bitset type") {
        WHEN ("constructing a Bitset with bit function") {
            constexpr TestType test_obj = TestType::template bit<3>();

            THEN ("the object has only that bit set") {
                REQUIRE(test_obj.value() == (1 << 3));
            }
        }
    }
}

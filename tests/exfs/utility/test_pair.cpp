#include "exfs/utility/pair.hpp"

#include <string>
#include <type_traits>
#include <utility>

#include <catch2/catch.hpp>

TEMPLATE_PRODUCT_TEST_CASE(
    "exfs::utility::pair - member aliases",
    "[unit][std-parity][utility-pair]",
    (std::pair, exfs::utility::pair),
    ((int const, std::string&))
) {
    GIVEN("a concrete pair type to test") {
        using Pair = TestType;

        THEN("the first type is a `int&`") {
            CHECK(std::is_same_v<typename Pair::first_type, int const>);
        }

        THEN("the second type is a `std::string const`") {
            CHECK(std::is_same_v<typename Pair::second_type, std::string&>);
        }
    }
}

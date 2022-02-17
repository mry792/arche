#include "exfs/iterator/traits.hpp"

#include <cstddef>
#include <iterator>
#include <type_traits>

#include <catch2/catch.hpp>

struct Empty {};

///
/// struct incrementable_traits
///

TEMPLATE_PRODUCT_TEST_CASE(
    "exfs::iterator::incrementable_traits - primary template",
    "[unit][std-parity][iterator]",
    (std::incrementable_traits, exfs::iterator::incrementable_traits),
    (Empty, Empty const)
) {
    CHECK(not requires { typename TestType::difference_type; });
}

TEMPLATE_PRODUCT_TEST_CASE(
    "exfs::iterator::incrementable_traits - pointers",
    "[unit][std-parity][iterator]",
    (std::incrementable_traits, exfs::iterator::incrementable_traits),
    (Empty*, int*, Empty**, Empty* const, int* const, Empty const*)
) {
    CHECK(std::is_same_v<typename TestType::difference_type, std::ptrdiff_t>);
}

struct Member_Type { struct difference_type; };

TEMPLATE_PRODUCT_TEST_CASE(
    "exfs::iterator::incrementable_traits - with member difference_type",
    "[unit][std-parity][iterator]",
    (std::incrementable_traits, exfs::iterator::incrementable_traits),
    (Member_Type, Member_Type const)
) {
    CHECK(std::is_same_v<
        typename TestType::difference_type,
        Member_Type::difference_type
    >);
}

#include "exfs/iterator/category_tags.hpp"

#include <iterator>
#include <type_traits>

#include <catch2/catch.hpp>

TEST_CASE(
    "exfs::iterator::*_tag",
    "[unit][std-parity][iterator]"
) {
    #define DO_TEST(NAMESPACE) \
        using input = NAMESPACE::input_iterator_tag; \
        using forward = NAMESPACE::forward_iterator_tag; \
        using bidirectional = NAMESPACE::bidirectional_iterator_tag; \
        using random_access = NAMESPACE::random_access_iterator_tag; \
        using contiguous = NAMESPACE::contiguous_iterator_tag; \
        CHECK(std::is_base_of_v<input, forward>); \
        CHECK(std::is_base_of_v<forward, bidirectional>); \
        CHECK(std::is_base_of_v<bidirectional, random_access>); \
        CHECK(std::is_base_of_v<random_access, contiguous>);

    SECTION("std") {
        DO_TEST(std);
    }

    SECTION("exfs") {
        DO_TEST(exfs::iterator);
    }

    #undef DO_TEST
}

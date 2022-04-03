#include "exfs/memory/storage.hpp"

#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

#include <catch2/catch.hpp>

TEMPLATE_TEST_CASE (
    "exfs::memory::storage - type properties",
    "[uni][memory]",
    int,
    std::uint64_t,
    std::string,
    std::vector<double>,
    std::int16_t[5]
) {
    using Storage = exfs::memory::storage<TestType>;

    CHECK(std::is_trivial_v<Storage>);
    CHECK(std::is_standard_layout_v<Storage>);
    CHECK(alignof(Storage) == alignof(TestType));
    CHECK(sizeof(Storage) == sizeof(TestType));
}

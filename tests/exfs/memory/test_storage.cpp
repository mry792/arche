#include "exfs/memory/storage.hpp"

#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

#include <catch2/catch.hpp>

#include "exfs/testing/Regular_Object.hpp"
#include "exfs/utility/functions.hpp"

TEMPLATE_TEST_CASE (
    "exfs::memory::storage - type properties",
    "[uni][memory]",
    int,
    std::uint64_t,
    std::string,
    std::vector<double>,
    std::int16_t[5],
    exfs::testing::Regular_Object
) {
    using Storage = exfs::memory::storage<TestType>;

    CHECK(std::is_trivial_v<Storage>);
    CHECK(std::is_standard_layout_v<Storage>);
    CHECK(alignof(Storage) == alignof(TestType));
    CHECK(sizeof(Storage) == sizeof(TestType));
}

SCENARIO (
    "Objects can be created and destroyed in `storage`.",
    "[unit][memory]"
) {
    using exfs::testing::Regular_Object;
    auto mock_obj = Regular_Object::initialize();

    GIVEN ("storage for a `Regular_Object`") {
        exfs::memory::storage<Regular_Object> storage;

        WHEN ("default constructing an object") {
            REQUIRE_CALL(*mock_obj, default_construct(0, 0));
            storage.construct();

            THEN ("destroying the objct calls the destructor") {
                REQUIRE_CALL(*mock_obj, destruct(0, 0));
                storage.destroy();
            }
        }

        AND_GIVEN ("another `Regular_Object`") {
            REQUIRE_CALL(*mock_obj, default_construct(0, 0));
            REQUIRE_CALL(*mock_obj, destruct(0, 0));
            Regular_Object src_obj{};

            WHEN ("copy constructing an object") {
                REQUIRE_CALL(*mock_obj, copy_construct(1, 0));
                storage.construct(src_obj);

                THEN ("destroying the objct calls the destructor") {
                    REQUIRE_CALL(*mock_obj, destruct(1, 0));
                    storage.destroy();
                }
            }

            WHEN ("move constructing an object") {
                REQUIRE_CALL(*mock_obj, move_construct(1, 0));
                storage.construct(exfs::move(src_obj));

                THEN ("destroying the objct calls the destructor") {
                    REQUIRE_CALL(*mock_obj, destruct(1, 0));
                    storage.destroy();
                }
            }
        }
    }
}

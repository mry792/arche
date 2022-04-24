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

            THEN ("the object is initialized") {
                CHECK(storage.object().id() == 0);
                CHECK(storage.object().data() == 0);
            }

            AND_THEN ("destroying the objct calls the destructor") {
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

                THEN ("the object is initialized") {
                    CHECK(storage.object().id() == 1);
                    CHECK(storage.object().data() == 0);
                }

                AND_THEN ("destroying the objct calls the destructor") {
                    REQUIRE_CALL(*mock_obj, destruct(1, 0));
                    storage.destroy();
                }
            }

            WHEN ("move constructing an object") {
                REQUIRE_CALL(*mock_obj, move_construct(1, 0));
                storage.construct(exfs::move(src_obj));

                THEN ("the object is initialized") {
                    CHECK(storage.object().id() == 1);
                    CHECK(storage.object().data() == 0);
                }

                AND_THEN ("destroying the objct calls the destructor") {
                    REQUIRE_CALL(*mock_obj, destruct(1, 0));
                    storage.destroy();
                }
            }
        }
    }
}

SCENARIO (
    "Constructed objects can be accessed by the overloaded accessor.",
    "[unit][memory]"
) {
    using exfs::testing::Regular_Object;
    auto mock_obj = Regular_Object::initialize();

    GIVEN ("populated storage for a `Regular_Object`") {
        exfs::memory::storage<Regular_Object> storage;
        REQUIRE_CALL(*mock_obj, default_construct(0, 0));
        storage.construct();

        WHEN ("accessing the object via lvalue") {
            REQUIRE_CALL(*mock_obj, copy_construct(1, 0));
            REQUIRE_CALL(*mock_obj, destruct(1, 0));
            using return_type = decltype(storage.object());
            Regular_Object result = storage.object();

            THEN ("the return type is an lvalue reference") {
                CHECK(std::is_same_v<return_type, Regular_Object&>);
            }

            THEN ("the value is copied") {
                CHECK(result.id() == 1);
                CHECK(result.data() == 0);
            }
        }

        WHEN ("accessing the object via const lvalue") {
            REQUIRE_CALL(*mock_obj, copy_construct(1, 0));
            REQUIRE_CALL(*mock_obj, destruct(1, 0));
            using return_type = decltype(exfs::as_const(storage).object());
            Regular_Object result = exfs::as_const(storage).object();

            THEN ("the return type is an lvalue reference") {
                CHECK(std::is_same_v<return_type, Regular_Object const&>);
            }

            THEN ("the value is copied") {
                CHECK(result.id() == 1);
                CHECK(result.data() == 0);
            }
        }

        WHEN ("accessing the object via rvalue") {
            REQUIRE_CALL(*mock_obj, move_construct(1, 0));
            REQUIRE_CALL(*mock_obj, destruct(1, 0));
            using return_type = decltype(exfs::move(storage).object());
            Regular_Object result = exfs::move(storage).object();

            THEN ("the return type is an lvalue reference") {
                CHECK(std::is_same_v<return_type, Regular_Object&&>);
            }

            THEN ("the value is copied") {
                CHECK(result.id() == 1);
                CHECK(result.data() == 0);
            }
        }
    }
}

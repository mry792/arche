#include "exfs/static_vector.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include <catch2/catch.hpp>

#include "exfs/iterator/reverse_iterator.hpp"
#include "exfs/testing/Regular_Object.hpp"
#include "exfs/utility/functions.hpp"

TEMPLATE_TEST_CASE (
    "exfs::static_vector - member aliases",
    "[unit][static_vector]",
    int,
    std::string,
    std::vector<double>,
    std::ostringstream
) {
    using Static_Vector = exfs::static_vector<TestType, 7u>;

    #define DO_CHECK(NAME, EXPECTED) \
    CHECK(std::is_same_v<typename Static_Vector::NAME, EXPECTED>);

    DO_CHECK(value_type,      TestType);
    DO_CHECK(pointer,         TestType*);
    DO_CHECK(const_pointer,   TestType const*);
    DO_CHECK(reference,       TestType&);
    DO_CHECK(const_reference, TestType const&);
    DO_CHECK(size_type,       std::size_t);
    DO_CHECK(difference_type, std::make_signed_t<std::size_t>);

    #undef DO_CHECK
}

SCENARIO (
    "exfs::static_vector - constructors",
    "[unit][static_vector]"
) {
    using exfs::testing::Regular_Object;
    auto mock_obj = Regular_Object::initialize();

    using Container = exfs::static_vector<Regular_Object, 5u>;

    #define REQUIRE_OBJECT_LIFETIME(ORIGIN, ID, DATA)                          \
    REQUIRE_CALL(*mock_obj, ORIGIN ## _construct(ID, DATA));                   \
    REQUIRE_CALL(*mock_obj, destruct(ID, DATA));

    WHEN ("default constructing a container") {
        Container container{};

        THEN ("no objects are constructed and the container is empty") {
            CHECK(container.empty());
            CHECK(container.size() == 0u);
        }
    }

    WHEN ("constructing a container with 3 initial objects") {
        REQUIRE_OBJECT_LIFETIME(default, 0, 0);
        REQUIRE_OBJECT_LIFETIME(default, 1, 1);
        REQUIRE_OBJECT_LIFETIME(default, 2, 2);

        Container container{3u};

        THEN ("the container has 3 objects") {
            CHECK(not container.empty());
            CHECK(container.size() == 3u);
        }
    }

    GIVEN ("an initial element") {
        REQUIRE_OBJECT_LIFETIME(default, 0, 0);
        Regular_Object obj{};

        WHEN ("constructing a container with 2 object copies") {
            REQUIRE_OBJECT_LIFETIME(copy, 1, 0);
            REQUIRE_OBJECT_LIFETIME(copy, 2, 0);

            Container container{2u, obj};

            THEN ("the container has 2 objects") {
                CHECK(not container.empty());
                CHECK(container.size() == 2u);
            }
        }
    }

    GIVEN ("an initial range") {
        REQUIRE_OBJECT_LIFETIME(default, 0, 0);
        REQUIRE_OBJECT_LIFETIME(default, 1, 1);
        std::array<Regular_Object, 2u> src{};

        WHEN ("constructing a container from the source range") {
            REQUIRE_OBJECT_LIFETIME(copy, 2, 0);
            REQUIRE_OBJECT_LIFETIME(copy, 3, 1);

            Container container{src.begin(), src.end()};

            THEN ("the container has the same number of objects") {
                CHECK(not container.empty());
                CHECK(container.size() == src.size());
            }
        }
    }

    GIVEN ("a container of 3 initial objects") {
        REQUIRE_CALL(*mock_obj, default_construct(0, 0));
        REQUIRE_CALL(*mock_obj, default_construct(1, 1));
        REQUIRE_CALL(*mock_obj, default_construct(2, 2));
        Container src{3u};

        WHEN ("constructing a container as a copy") {
            REQUIRE_OBJECT_LIFETIME(copy, 3, 0);
            REQUIRE_OBJECT_LIFETIME(copy, 4, 1);
            REQUIRE_OBJECT_LIFETIME(copy, 5, 2);

            Container container{src};

            THEN ("the container has the same number of objects") {
                CHECK(not container.empty());
                CHECK(container.size() == src.size());
            }
        }

        // The elements of the source container aren't destroyed till well after
        // the copy has been made.
        REQUIRE_CALL(*mock_obj, destruct(0, 0));
        REQUIRE_CALL(*mock_obj, destruct(1, 1));
        REQUIRE_CALL(*mock_obj, destruct(2, 2));
        src.clear();
    }

    GIVEN ("a container of 3 initial objects") {
        REQUIRE_CALL(*mock_obj, default_construct(0, 0));
        REQUIRE_CALL(*mock_obj, default_construct(1, 1));
        REQUIRE_CALL(*mock_obj, default_construct(2, 2));
        Container src{3u};

        WHEN ("constructing a container by move") {
            REQUIRE_CALL(*mock_obj, move_construct(3, 0));
            REQUIRE_CALL(*mock_obj, move_construct(4, 1));
            REQUIRE_CALL(*mock_obj, move_construct(5, 2));
            REQUIRE_CALL(*mock_obj, destruct(0, 0));
            REQUIRE_CALL(*mock_obj, destruct(1, 1));
            REQUIRE_CALL(*mock_obj, destruct(2, 2));

            Container container{exfs::move(src)};

            THEN ("the container has the same number of objects") {
                CHECK(not container.empty());
                CHECK(container.size() == 3u);
            }

            THEN ("the source container is empty") {
                CHECK(src.empty());
                CHECK(src.size() == 0u);
            }

            REQUIRE_CALL(*mock_obj, destruct(3, 0));
            REQUIRE_CALL(*mock_obj, destruct(4, 1));
            REQUIRE_CALL(*mock_obj, destruct(5, 2));
            container.clear();
        }
    }

    #undef REQUIRE_OBJECT_LIFETIME
}

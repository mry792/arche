#include "exfs/static_vector.hpp"

#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include <catch2/catch.hpp>

#include "exfs/iterator/reverse_iterator.hpp"
#include "exfs/testing/Regular_Object.hpp"

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

    #undef REQUIRE_OBJECT_LIFETIME
}

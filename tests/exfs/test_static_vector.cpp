#include "exfs/static_vector.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

#include "exfs/iterator/reverse_iterator.hpp"
#include "exfs/testing/Regular_Object.hpp"
#include "exfs/utility/functions.hpp"

#define REQUIRE_OBJECT_LIFETIME(ORIGIN, ID, DATA)                              \
REQUIRE_CALL(*mock_obj, ORIGIN ## _construct(ID, DATA));                       \
REQUIRE_CALL(*mock_obj, destruct(ID, DATA));

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

            CHECK(container[0].id() == 0);
            CHECK(container[1].id() == 1);
            CHECK(container[2].id() == 2);
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

                CHECK(container[0].id() == 1);
                CHECK(container[1].id() == 2);
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

                CHECK(container[0].id() == 2);
                CHECK(container[1].id() == 3);
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

                CHECK(container[0].id() == 3);
                CHECK(container[1].id() == 4);
                CHECK(container[2].id() == 5);
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

                CHECK(container[0].id() == 3);
                CHECK(container[1].id() == 4);
                CHECK(container[2].id() == 5);
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

    WHEN ("construcing a container from an initializer list with 2 elements") {
        REQUIRE_OBJECT_LIFETIME(default, 0, 0);
        REQUIRE_OBJECT_LIFETIME(default, 1, 1);
        REQUIRE_OBJECT_LIFETIME(copy, 2, 0);
        REQUIRE_OBJECT_LIFETIME(copy, 3, 1);

        Container container{{Regular_Object{}, Regular_Object{}}};

        THEN ("the container has two elements") {
            CHECK(not container.empty());
            CHECK(container.size() == 2u);

            CHECK(container[0].id() == 2);
            CHECK(container[1].id() == 3);
        }
    }
}

SCENARIO (
    "exfs::static_vector - assigment",
    "[unit][static_vector]"
) {
    using exfs::testing::Regular_Object;
    using Container = exfs::static_vector<Regular_Object, 3u>;
    auto mock_obj = Regular_Object::initialize();

    GIVEN ("a container of 2 elements") {
        REQUIRE_CALL(*mock_obj, default_construct(0, 0));
        REQUIRE_CALL(*mock_obj, default_construct(1, 1));
        Container container{2u};

        AND_GIVEN ("a smaller container (1 element)") {
            REQUIRE_CALL(*mock_obj, default_construct(2, 2));
            Container src{1u};

            WHEN ("copy assigning") {
                REQUIRE_CALL(*mock_obj, copy_assign(0, 0, 2, 2));
                REQUIRE_CALL(*mock_obj, destruct(1, 1));

                container = src;

                THEN ("the two containers are equal") {
                    REQUIRE(container.size() == src.size());
                    for (unsigned idx = 0u; idx < container.size(); ++idx) {
                        CAPTURE(idx);
                        CHECK(container[idx].data() == src[idx].data());
                    }
                }

                REQUIRE_CALL(*mock_obj, destruct(2, 2));
                src.clear();

                REQUIRE_CALL(*mock_obj, destruct(0, 2));
                container.clear();
            }
        }

        AND_GIVEN ("a larger container (3 elements)") {
            REQUIRE_CALL(*mock_obj, default_construct(2, 2));
            REQUIRE_CALL(*mock_obj, default_construct(3, 3));
            REQUIRE_CALL(*mock_obj, default_construct(4, 4));
            Container src{3u};

            WHEN ("copy assigning") {
                REQUIRE_CALL(*mock_obj, copy_assign(0, 0, 2, 2));
                REQUIRE_CALL(*mock_obj, copy_assign(1, 1, 3, 3));
                REQUIRE_CALL(*mock_obj, copy_construct(5, 4));
                container = src;

                THEN ("the two containers are equal") {
                    REQUIRE(container.size() == src.size());
                    for (unsigned idx = 0u; idx < container.size(); ++idx) {
                        CAPTURE(idx);
                        CHECK(container[idx].data() == src[idx].data());
                    }
                }

                REQUIRE_CALL(*mock_obj, destruct(2, 2));
                REQUIRE_CALL(*mock_obj, destruct(3, 3));
                REQUIRE_CALL(*mock_obj, destruct(4, 4));
                src.clear();

                REQUIRE_CALL(*mock_obj, destruct(0, 2));
                REQUIRE_CALL(*mock_obj, destruct(1, 3));
                REQUIRE_CALL(*mock_obj, destruct(5, 4));
                container.clear();
            }
        }
    }
}

SCENARIO (
    "exfs::static_vector - accessors",
    "[unit][static_vector]"
) {
    using namespace std::literals::string_literals;
    using Container = exfs::static_vector<std::string, 4u>;

    GIVEN ("a container of 3 elements") {
        Container container{{"first"s, "second"s, "third"s}};

        THEN ("the index operator accesses an element") {
            CHECK(container[1] == "second"s);
            CHECK(std::is_same_v<decltype(container[1]), std::string&>);

            AND_WHEN ("the element is changed externally") {
                auto& elem = container[0];
                elem += "_blah";

                THEN ("that change is reflected in the container") {
                    CHECK(container[0] == "first_blah"s);
                }
            }
        }

        THEN ("the `front` function accesses the first element") {
            CHECK(container.front() == "first"s);
            CHECK(std::is_same_v<decltype(container.front()), std::string&>);

            AND_WHEN ("the element is changed externally") {
                auto& elem = container.front();
                elem += "_blah";

                THEN ("that change is reflected in the container") {
                    CHECK(container[0] == "first_blah"s);
                }
            }
        }

        THEN ("the `back` function accesses the last element") {
            CHECK(container.back() == "third"s);
            CHECK(std::is_same_v<decltype(container.back()), std::string&>);

            AND_WHEN ("the element is changed externally") {
                auto& elem = container.back();
                elem += "_blah";

                THEN ("that change is reflected in the container") {
                    CHECK(container[2] == "third_blah"s);
                }
            }
        }

        AND_GIVEN ("a const reference to that container") {
            Container const& ccont = container;

            THEN ("the const index operator accesses an element") {
                CHECK(ccont[2] == "third"s);
                CHECK(std::is_same_v<decltype(ccont[2]), std::string const&>);
            }

            THEN ("the const front function accesses the first element") {
                CHECK(ccont.front() == "first"s);
                CHECK(std::is_same_v<decltype(ccont.front()), std::string const&>);
            }

            THEN ("the const back function accesses the last element") {
                CHECK(ccont.back() == "third"s);
                CHECK(std::is_same_v<decltype(ccont.back()), std::string const&>);
            }
        }
    }

    GIVEN ("a container of 1 element") {
        Container container{{"sole element"s}};
        Container const& ccont = container;

        THEN ("`front()` and `back()` refer to the same element") {
            CHECK(&container.front() == &container.back());
            CHECK(&container.front() == &ccont.back());
            CHECK(&ccont.front() == &container.back());
            CHECK(&ccont.front() == &ccont.back());
        }
    }
}

#undef REQUIRE_OBJECT_LIFETIME

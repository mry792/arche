#include "exfs/utility/pair.hpp"

#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include <catch2/catch.hpp>
#include <catch2/trompeloeil.hpp>

#include "exfs/testing/Regular_Object.hpp"

TEMPLATE_TEST_CASE (
    "exfs::utility::pair - member aliases",
    "[unit][std-parity][utility-pair]",
    (std::pair<int const&, std::string>),
    (exfs::utility::pair<int const&, std::string>)
) {
    GIVEN ("a concrete pair type to test") {
        using Pair = TestType;

        THEN ("the first type is a `int const&`") {
            CHECK(std::is_same_v<typename Pair::first_type, int const&>);
        }

        THEN ("the second type is a `std::string`") {
            CHECK(std::is_same_v<typename Pair::second_type, std::string>);
        }
    }
}

template <typename T_Old_Full, typename... T_New_Inner>
struct rebind;

template <typename T_Old_Full, typename... T_New_Inner>
using rebind_t = typename rebind<T_Old_Full, T_New_Inner...>::type;

template <
    template <typename...> typename T_Outer,
    typename... T_New_Inner,
    typename... T_Old_Inner
>
struct rebind<T_Outer<T_Old_Inner...>, T_New_Inner...> {
    using type = T_Outer<T_New_Inner...>;
};


using exfs::testing::Regular_Object;

TEMPLATE_TEST_CASE (
    "exfs::utility::pair - constructors",
    "[unit][std-parity][utility-pair]",
    (std::pair<Regular_Object, Regular_Object const&>),
    (exfs::utility::pair<Regular_Object, Regular_Object const&>)
) {
    using Pair = TestType;
    using trompeloeil::_;
    auto mock_obj = Regular_Object::initialize();

    // Allow arbitrary destruction.
    ALLOW_CALL(*mock_obj, destruct(_, _));

    GIVEN ("constituent values") {
        REQUIRE_CALL(*mock_obj, default_construct(0, 0));
        REQUIRE_CALL(*mock_obj, default_construct(1, 1));

        Regular_Object obj_0;
        Regular_Object obj_1;

        WHEN ("constructed with two value types") {
            // Object-0 is copied. Object-1 is not.
            REQUIRE_CALL(*mock_obj, copy_construct(2, 0));

            Pair pair{obj_0, obj_1};

            THEN ("the types of member variables match the member aliases") {
                CHECK(std::is_same_v<decltype(pair.first), Regular_Object>);
                CHECK(std::is_same_v<
                    decltype(pair.second),
                    Regular_Object const&
                >);
            }

            THEN ("the first value is equal to the copied value") {
                CHECK(pair.first == obj_0);
            }

            THEN ("the second value is a reference to the local variable") {
                CHECK(pair.second == obj_1);
                CHECK(&pair.second == &obj_1);
            }
        }

        WHEN ("constructed by moving into the first (value) type") {
            // Object-0 is moved. Object-1 is not.
            REQUIRE_CALL(*mock_obj, move_construct(2, 0));

            Pair pair{std::move(obj_0), obj_1};

            THEN ("the first value is equal to the copied value") {
                CHECK(pair.first == obj_0);
            }

            THEN ("the second value is a reference to the local variable") {
                CHECK(pair.second == obj_1);
                CHECK(&pair.second == &obj_1);
            }
        }

        WHEN ("constructed by moving into the second (reference) type") {
            // Object-0 is copied. Object-1 is not.
            REQUIRE_CALL(*mock_obj, copy_construct(2, 0));

            Pair pair{obj_0, std::move(obj_1)};

            THEN ("the first value is equal to the copied value") {
                CHECK(pair.first == obj_0);
            }

            THEN ("the second value is a reference to the local variable") {
                CHECK(pair.second == obj_1);
                CHECK(&pair.second == &obj_1);
            }
        }
    }

    GIVEN ("a source pair of the same type") {
        REQUIRE_CALL(*mock_obj, default_construct(0, 0));
        REQUIRE_CALL(*mock_obj, default_construct(1, 1));
        REQUIRE_CALL(*mock_obj, move_construct(2, 1));

        // Referenced object.
        Regular_Object obj_0;

        Pair src_pair{Regular_Object{}, obj_0};

        WHEN ("constructed with lvalue of existing pair") {
            // The first item (object-1, a value) is copied. The second
            // (reference) item (object-0, a reference) is not.
            REQUIRE_CALL(*mock_obj, copy_construct(3, 1));

            Pair result_pair{src_pair};

            THEN ("the first value is equal to the copied value") {
                CHECK(result_pair.first == src_pair.first);
            }

            THEN ("the second value is a reference to the local variable") {
                CHECK(result_pair.second == obj_0);
                CHECK(&result_pair.second == &obj_0);
            }
        }

        WHEN ("constructed with rvalue of existing pair") {
            // The first item (object-1, a value) is moved. The second
            // (reference) item (object-0, a reference) is not.
            REQUIRE_CALL(*mock_obj, move_construct(3, 1));

            Pair result_pair{std::move(src_pair)};

            THEN ("the first value is equal to the moved value") {
                CHECK(result_pair.first == src_pair.first);
            }

            THEN ("the second value is a reference to the local variable") {
                CHECK(result_pair.second == obj_0);
                CHECK(&result_pair.second == &obj_0);
            }
        }
    }

    GIVEN ("a source pair of lvalue references") {
        REQUIRE_CALL(*mock_obj, default_construct(0, 0));
        REQUIRE_CALL(*mock_obj, default_construct(1, 1));

        Regular_Object obj_0;
        Regular_Object obj_1;

        using Src_Pair = rebind_t<Pair, Regular_Object&, Regular_Object&>;

        Src_Pair src_pair{obj_0, obj_1};

        WHEN ("constructed with lvalue of existing pair") {
            // Object-0 is copied. Object-1 is not.
            REQUIRE_CALL(*mock_obj, copy_construct(2, 0));

            Pair result_pair{src_pair};

            THEN ("the first value is equal to the copied value") {
                CHECK(result_pair.first == src_pair.first);
            }

            THEN ("the second value is a reference to the local variable") {
                CHECK(result_pair.second == src_pair.second);
                CHECK(&result_pair.second == &src_pair.second);
            }
        }

        WHEN ("constructed with rvalue of existing pair") {
            // Object-0 is copied due to reference collapsing rules. Object-1,
            // as a reference, is neither copied nor moved.
            REQUIRE_CALL(*mock_obj, copy_construct(2, 0));

            Pair result_pair{std::move(src_pair)};

            THEN ("the first value is equal to the moved value") {
                CHECK(result_pair.first == src_pair.first);
            }

            THEN ("the second value is a reference to the local variable") {
                CHECK(result_pair.second == src_pair.second);
                CHECK(&result_pair.second == &src_pair.second);
            }
        }
    }

    GIVEN ("a source pair of rvalue references") {
        REQUIRE_CALL(*mock_obj, default_construct(0, 0));
        REQUIRE_CALL(*mock_obj, default_construct(1, 1));

        Regular_Object obj_0;
        Regular_Object obj_1;

        using Src_Pair = rebind_t<Pair, Regular_Object&&, Regular_Object&&>;

        Src_Pair src_pair{std::move(obj_0), std::move(obj_1)};

        WHEN ("constructed with lvalue of existing pair") {
            // Object-0 is copied. Object-1 is not
            REQUIRE_CALL(*mock_obj, copy_construct(2, 0));

            Pair result_pair{src_pair};

            THEN ("the first value is equal to the copied value") {
                CHECK(result_pair.first == src_pair.first);
            }

            THEN ("the second value is a reference to the local variable") {
                CHECK(result_pair.second == src_pair.second);
                CHECK(&result_pair.second == &src_pair.second);
            }
        }

        WHEN ("constructed with rvalue of existing pair") {
            // Object-0 is moved due to reference collapsing rules. Object-1, as
            // a reference, is neither copied nor moved.
            REQUIRE_CALL(*mock_obj, move_construct(2, 0));

            Pair result_pair{std::move(src_pair)};

            THEN ("the first value is equal to the moved value") {
                CHECK(result_pair.first == src_pair.first);
            }

            THEN ("the second value is a reference to the local variable") {
                CHECK(result_pair.second == src_pair.second);
                CHECK(&result_pair.second == &src_pair.second);
            }
        }
    }
}

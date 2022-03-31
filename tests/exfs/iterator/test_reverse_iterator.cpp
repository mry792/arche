#include "exfs/iterator/reverse_iterator.hpp"

#include <iterator>
#include <list>
#include <tuple>
#include <type_traits>
#include <vector>

#include <catch2/catch.hpp>

#include "exfs/iterator/concepts.hpp"
#include "exfs/iterator/category_tags.hpp"
#include "exfs/iterator/operations.hpp"

#include "models.hpp"

namespace {
template <typename T>
struct tag {
    using type = T;
};

template <typename T>
constexpr tag<T> tag_c{};

template <template <typename Adaptee_> typename Adapter>
struct Wrapper {
    template <typename Adaptee>
    using type = Adapter<Adaptee>;
};

using Std_Reverse_Iterator = Wrapper<std::reverse_iterator>;
using Exfs_Reverse_Iterator = Wrapper<exfs::iterator::reverse_iterator>;
}  //

TEMPLATE_TEST_CASE (
    "exfs::iterator::reverse_iterator - member aliases",
    "[unit][std-parity][iterator]",
    Std_Reverse_Iterator,
    Exfs_Reverse_Iterator
) {
    using exfs::iterator::models::Value;

    auto do_test = [](
        auto base_iter_tag,
        auto expected_cat_tag,
        auto expected_ptr_tag
    ) {
        using Base_Iterator = typename decltype(base_iter_tag)::type;
        using Reverse_Iterator = typename TestType::type<Base_Iterator>;

        using Expected_Category = typename decltype(expected_cat_tag)::type;
        using Expected_Pointer = typename decltype(expected_ptr_tag)::type;

        THEN ("it has the expected member aliases") {
            CHECK(std::is_same_v<
                typename Reverse_Iterator::iterator_type,
                Base_Iterator
            >);
            CHECK(std::is_same_v<
                typename Reverse_Iterator::iterator_concept,
                Expected_Category
            >);
            CHECK(std::is_same_v<
                typename Reverse_Iterator::iterator_category,
                Expected_Category
            >);
            CHECK(std::is_same_v<
                typename Reverse_Iterator::value_type,
                Value
            >);
            CHECK(std::is_same_v<
                typename Reverse_Iterator::difference_type,
                std::ptrdiff_t
            >);

            // NOTE: A `pointer` alias is not a requirement to model
            //       `random_access_iterator`.
            CHECK(std::is_same_v<
                typename Reverse_Iterator::pointer,
                Expected_Pointer
            >);

            CHECK(std::is_same_v<
                typename Reverse_Iterator::reference,
                Value&
            >);
        }
    };

    GIVEN ("a raw pointer reverse iterator") {
        using exfs::iterator::models::Value;
        using Iter_Tag = exfs::iterator::random_access_iterator_tag;

        do_test(tag_c<Value*>, tag_c<Iter_Tag>, tag_c<Value*>);
    }

    GIVEN ("a random-access reverse iterator") {
        using Iter_Model = exfs::iterator::models::random_access_iterator;
        using Iter_Tag = exfs::iterator::random_access_iterator_tag;

        do_test(tag_c<Iter_Model>, tag_c<Iter_Tag>, tag_c<void>);
    }

    GIVEN ("a bidirectional reverse iterator") {
        using Iter_Model = exfs::iterator::models::bidirectional_iterator;
        using Iter_Tag = exfs::iterator::bidirectional_iterator_tag;

        do_test(tag_c<Iter_Model>, tag_c<Iter_Tag>, tag_c<void>);
    }
}

TEMPLATE_TEST_CASE (
    "exfs::iterator::reverse_iterator - constructors",
    "[unit][std-parity][iterator]",
    Std_Reverse_Iterator,
    Exfs_Reverse_Iterator
) {
    auto do_test = [] (auto base_iter, auto const_base_iter_tag) {
        using Base_Iterator = decltype(base_iter);
        using Reverse_Iterator = typename TestType::type<Base_Iterator>;

        WHEN ("default construct a reverse_iterator") {
            Reverse_Iterator rev_iter{};

            THEN ("the `base` property is also default constructed") {
                CHECK(rev_iter.base() == Base_Iterator{});
                CHECK(rev_iter.base() != base_iter);
            }
        }

        WHEN ("construct a reverse_iterator from the base iterator") {
            Reverse_Iterator rev_iter{base_iter};

            THEN ("the `base` property is equal to the original base iterator") {
                CHECK(rev_iter.base() != Base_Iterator{});
                CHECK(rev_iter.base() == base_iter);
            }
        }

        AND_GIVEN ("an existing reverse_iterator") {
            Reverse_Iterator rev_iter{base_iter};

            WHEN ("copy construct the reverse_iterator") {
                Reverse_Iterator new_rev_iter{rev_iter};

                THEN ("the `base` properties are the same") {
                    CHECK(new_rev_iter.base() == rev_iter.base());
                }
            }

            WHEN ("move construct the reverse_iterator") {
                Reverse_Iterator new_rev_iter{exfs::move(rev_iter)};

                THEN ("the `base` property is the same as the original base") {
                    CHECK(new_rev_iter.base() == base_iter);
                }
            }

            AND_GIVEN ("const iterator and const reverse iterator types") {
                using Const_Base_Iterator = decltype(const_base_iter_tag)::type;
                using Const_Reverse_Iterator =
                    typename TestType::type<Const_Base_Iterator>;

                static_assert(std::is_constructible_v<
                    Const_Base_Iterator,
                    Base_Iterator
                >);
                static_assert(not std::is_same_v<
                    std::remove_cvref_t<Const_Base_Iterator>,
                    std::remove_cvref_t<Base_Iterator>
                >);

                WHEN ("copy construct a const rev iter from a rev iter") {
                    Const_Reverse_Iterator const_rev_iter{rev_iter};

                    THEN ("the `base` property is the same") {
                        CHECK(const_rev_iter.base() == rev_iter.base());
                    }
                }

                WHEN ("move construct a const rev iter from a rev iter") {
                    Const_Reverse_Iterator const_rev_iter{exfs::move(rev_iter)};

                    THEN ("the `base` property is the same as the original base") {
                        CHECK(const_rev_iter.base() == base_iter);
                    }
                }
            }
        }
    };

    using exfs::iterator::models::Value;

    GIVEN ("a raw pointer iterator") {
        Value value;
        Value* base_iter = &value;

        do_test(base_iter, tag_c<Value const*>);
    }

    GIVEN ("a random-access iterator") {
        std::vector<Value> vect{3};

        do_test(vect.begin() + 1, tag_c<decltype(vect.cbegin())>);
    }

    GIVEN ("a bidirectional iterator") {
        std::list<Value> list;
        list.emplace_back();
        list.emplace_back();

        do_test(list.begin(), tag_c<decltype(list.cbegin())>);
    }
}

TEMPLATE_TEST_CASE (
    "exfs::iterator::reverse_iterator - assigment",
    "[unit][std-parity][iterator]",
    Std_Reverse_Iterator,
    Exfs_Reverse_Iterator
) {
    auto do_copy_move_test = [] (auto rev_iter, auto other_rev_iter) {
        REQUIRE(rev_iter.base() != other_rev_iter.base());

        WHEN ("copy assign the wrappers") {
            rev_iter = other_rev_iter;

            THEN ("the `base` property is the same") {
                CHECK(rev_iter.base() == other_rev_iter.base());
            }
        }

        WHEN ("move assign the wrappers") {
            auto const other_rev_iter_base = other_rev_iter.base();
            rev_iter = exfs::move(other_rev_iter);

            THEN ("the `base` property is the same as before") {
                CHECK(rev_iter.base() == other_rev_iter_base);
            }
        }
    };

    auto do_test = [do_copy_move_test] (auto base_iter, auto const_base_iter) {
        static_assert(not std::is_same_v<
            decltype(base_iter),
            decltype(const_base_iter)
        >);
        REQUIRE(base_iter == const_base_iter);

        using Reverse_Iterator = typename TestType::type<decltype(base_iter)>;
        using Const_Reverse_Iterator =
            typename TestType::type<decltype(const_base_iter)>;

        AND_GIVEN ("reverse iterator wrappers") {
            do_copy_move_test(
                Reverse_Iterator{exfs::iterator::next(base_iter)},
                Reverse_Iterator{base_iter}
            );
        }

        AND_GIVEN ("const reverse iterator and reverse iterator wrappers") {
            do_copy_move_test(
                Const_Reverse_Iterator{const_base_iter},
                Reverse_Iterator{exfs::iterator::next(base_iter)}
            );
        }
    };

    using exfs::iterator::models::Value;

    GIVEN ("a raw pointer iterator") {
        Value value[2];
        Value* base_iter = &value[0];
        Value const* const_base_iter = base_iter;

        do_test(base_iter, const_base_iter);
    }

    GIVEN ("a random-access iterator") {
        std::vector<Value> vect{3};

        do_test(vect.begin() + 1, vect.cbegin() + 1);
    }

    GIVEN ("a bidirectional iterator") {
        std::list<Value> list;
        list.emplace_back();
        list.emplace_back();

        do_test(list.begin(), list.cbegin());
    }
}

TEMPLATE_TEST_CASE (
    "exfs::iterator::reverse_iterator - access",
    "[unit][std-parity][iterator]",
    Std_Reverse_Iterator,
    Exfs_Reverse_Iterator
) {
    using exfs::iterator::models::Value;

    auto do_test = [] (auto container_tag) {
        using Container = typename decltype(container_tag)::type;
        Container container = {{-0.3}, {1.4}};

        using Base_Iterator = decltype(std::cend(container));
        Base_Iterator base_iter{std::cend(container)};

        AND_GIVEN ("a reverse iterator adaptor") {
            using Reverse_Iterator = typename TestType::type<Base_Iterator>;
            Reverse_Iterator rev_iter{base_iter};

            WHEN ("dereferenceing the reverse iterator (*)") {
                Value const& last_elem = *rev_iter;

                THEN ("the value is the expected item from the container") {
                    CHECK(last_elem.data == 1.4);
                }
            }

            WHEN ("dereferenceing the reverse iterator (->)") {
                double const& data = rev_iter->data;

                THEN ("the value is the expected data from the container") {
                    CHECK(data == 1.4);
                }
            }

            using exfs::iterator::random_access_iterator;
            if constexpr (random_access_iterator<Base_Iterator>) {
                auto [idx, expected_value] = GENERATE(table<int, double>(
                    {{0, 1.4}, {1, -0.3}}));

                WHEN ("accessing the index " << idx << " element") {
                    Value const& elem = rev_iter[idx];

                    THEN ("the value is " << expected_value) {
                        CHECK(elem.data == expected_value);
                    }
                }
            }
        }
    };

    GIVEN ("a raw pointer iterator") {
        do_test(tag_c<Value[2]>);
    }

    GIVEN ("a random-access iterator") {
        do_test(tag_c<std::vector<Value>>);
    }

    GIVEN ("a bidirectional iterator") {
        do_test(tag_c<std::list<Value>>);
    }
}

TEMPLATE_TEST_CASE (
    "exfs::iterator::reverse_iterator - modifiers",
    "[unit][std-parity][iterator]",
    Std_Reverse_Iterator,
    Exfs_Reverse_Iterator
) {
    using exfs::iterator::models::Value;

    auto do_test = [] (auto container_tag) {
        using Container = typename decltype(container_tag)::type;
        Container container = {{-0.3}, {1.7}};

        AND_GIVEN ("a reverse iterator adaptor pointing to the end") {
            using Base_Iterator = decltype(std::cbegin(container));
            Base_Iterator base_last =
                exfs::iterator::next(std::cbegin(container));
            Base_Iterator base_end = std::cend(container);

            using Reverse_Iterator = typename TestType::type<Base_Iterator>;
            Reverse_Iterator rev_iter{base_end};

            WHEN ("pre-incrementing") {
                auto pre_inc_result = ++rev_iter;

                THEN ("the iterator points to the beginning") {
                    CHECK(rev_iter.base() == base_last);
                }

                THEN ("the result points to the beginning") {
                    CHECK(pre_inc_result.base() == base_last);
                }

                AND_WHEN ("pre-decrementing") {
                    auto pre_dec_result = --rev_iter;

                    THEN ("the iterator points to the end") {
                        CHECK(rev_iter.base() == base_end);
                    }

                    THEN ("the result points to the end") {
                        CHECK(pre_dec_result.base() == base_end);
                    }
                }
            }

            WHEN ("post-incrementing") {
                auto post_inc_result = rev_iter++;

                THEN ("the iterator points to the beginning") {
                    CHECK(rev_iter.base() == base_last);
                }

                THEN ("the result still points to the end") {
                    CHECK(post_inc_result.base() == base_end);
                }

                AND_WHEN ("post-decrementing") {
                    auto post_dec_result = rev_iter--;

                    THEN ("the iterator points to the end") {
                        CHECK(rev_iter.base() == base_end);
                    }

                    THEN ("the result still points to the beginning") {
                        CHECK(post_dec_result.base() == base_last);
                    }
                }
            }
        }
    };

    GIVEN ("a raw array") {
        do_test(tag_c<Value[2]>);
    }

    GIVEN ("a random-access container (std::vector)") {
        do_test(tag_c<std::vector<Value>>);
    }

    GIVEN ("a bidirectional container (std::list)") {
        do_test(tag_c<std::list<Value>>);
    }
}

TEMPLATE_TEST_CASE (
    "exfs::iterator::reverse_iterator - arithmetic",
    "[unit][std-parity][iterator]",
    Std_Reverse_Iterator,
    Exfs_Reverse_Iterator
) {
    using exfs::iterator::models::Value;

    auto do_test = [] (auto container_tag) {
        using Container = typename decltype(container_tag)::type;
        Container container = {{-0.3}, {1.7}, {0.6}, {9.8}};

        AND_GIVEN ("a reverse iterator adaptor") {
            using Base_Iterator = decltype(std::cbegin(container));
            using Reverse_Iterator = typename TestType::type<Base_Iterator>;

            Reverse_Iterator const rev_iter{std::cbegin(container) + 2};

            AND_GIVEN ("a distance") {
                using difference_type = typename Reverse_Iterator::difference_type;
                difference_type const dist = GENERATE(range(-2, 2));
                INFO("distance = " << dist);

                WHEN ("operator + (rev_iter, dist)") {
                    auto result = rev_iter + dist;

                    THEN ("the result is `rev_iter.base() - dist`") {
                        CHECK(result.base() == rev_iter.base() - dist);
                    }
                }

                WHEN ("operator + (dist, rev_iter)") {
                    auto result = dist + rev_iter;

                    THEN ("the result is `rev_iter.base() - dist`") {
                        CHECK(result.base() == rev_iter.base() - dist);
                    }
                }

                WHEN ("operator - (rev_iter, dist)") {
                    auto result = rev_iter - dist;

                    THEN ("the result is `rev_iter.base() + dist`") {
                        CHECK(result.base() == rev_iter.base() + dist);
                    }
                }
            }

            AND_GIVEN ("another reverse iterator before `rev_iter`") {
                Reverse_Iterator const other_rev_iter{std::cbegin(container) + 3};

                WHEN ("operator - (rev_iter, other_rev_iter)") {
                    auto result = rev_iter - other_rev_iter;

                    THEN ("the result is `other_rev_iter.base() - rev_iter.base()`") {
                        CHECK(result == 1);
                    }
                }
            }

            AND_GIVEN ("another reverse iterator after `rev_iter`") {
                Reverse_Iterator const other_rev_iter{std::cbegin(container)};

                WHEN ("operator - (rev_iter, other_rev_iter)") {
                    auto result = rev_iter - other_rev_iter;

                    THEN ("the result is `other_rev_iter.base() - rev_iter.base()`") {
                        CHECK(result == -2);
                    }
                }
            }
        }
    };

    GIVEN ("a raw array") {
        do_test(tag_c<Value[4]>);
    }

    GIVEN ("a random-access container (std::vector)") {
        do_test(tag_c<std::vector<Value>>);
    }

    // Arithmetic operations all require random access iterators. This test case
    // doesn't include tests with bidirectional iterators.
}

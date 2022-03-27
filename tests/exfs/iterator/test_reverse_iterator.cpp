#include "exfs/iterator/reverse_iterator.hpp"

#include <iterator>
#include <list>
#include <type_traits>
#include <vector>

#include <catch2/catch.hpp>

#include "exfs/iterator/category_tags.hpp"

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

#include "exfs/iterator/reverse_iterator.hpp"

#include <iterator>
#include <type_traits>

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

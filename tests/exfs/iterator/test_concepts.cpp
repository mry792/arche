#include "exfs/iterator/concepts.hpp"

#include <forward_list>
#include <iterator>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include <catch2/catch.hpp>

namespace {
using Pointer_Types = std::tuple<
    int*,
    int const*,
    int volatile*,
    int const volatile*,
    int* const,
    int const* const,
    int volatile* const,
    int const volatile* const
>;

using Smart_Pointer_Types = std::tuple<
    std::unique_ptr<int>,
    std::unique_ptr<int const>,
    std::unique_ptr<int> const,
    std::unique_ptr<int const> const,
    std::shared_ptr<int>,
    std::shared_ptr<int const>,
    std::shared_ptr<int> const,
    std::shared_ptr<int const> const
>;

using Iterator_Types = std::tuple<
    typename std::forward_list<int>::iterator,
    typename std::forward_list<int>::const_iterator,
    typename std::forward_list<int>::iterator const,
    typename std::forward_list<int>::const_iterator const,
    typename std::vector<int>::iterator,
    typename std::vector<int>::const_iterator,
    typename std::vector<int>::iterator const,
    typename std::vector<int>::const_iterator const
>;

template <typename... Tuples>
using tuple_cat_result_t = decltype(std::tuple_cat(std::declval<Tuples&>()...));

using All_Test_Types = tuple_cat_result_t<
    Pointer_Types,
    Smart_Pointer_Types,
    Iterator_Types
>;
}  // namespace

#define CHECK_CONCEPT_PARITY(CONCEPT_NAME, MODEL)                              \
CHECK(std::CONCEPT_NAME<MODEL> == exfs::iterator::CONCEPT_NAME<MODEL>);

///
/// concept indirectly_readable
///

TEMPLATE_LIST_TEST_CASE(
    "exfs::iterator::indirectly_readable",
    "[unit][std-parity][iterator][concept]",
    All_Test_Types
) {
    CHECK_CONCEPT_PARITY(indirectly_readable, TestType);
}

///
/// concept indirectly_writable
///

TEMPLATE_LIST_TEST_CASE(
    "exfs::iterator::indirectly_writable",
    "[unit][std-parity][iterator][concept]",
    All_Test_Types
) {
    CHECK((std::indirectly_writable<TestType, int>) ==
        (exfs::iterator::indirectly_writable<TestType, int>));
}

#undef CHECK_CONCEPT_PARITY

#include "exfs/iterator/traits.hpp"

#include <cstddef>
#include <iterator>
#include <list>
#include <map>
#include <tuple>
#include <type_traits>
#include <vector>

#include <catch2/catch.hpp>

struct Empty {};

///
/// struct incrementable_traits
///

TEMPLATE_PRODUCT_TEST_CASE (
    "exfs::iterator::incrementable_traits - primary template",
    "[unit][std-parity][iterator]",
    (std::incrementable_traits, exfs::iterator::incrementable_traits),
    (Empty, Empty const)
) {
    CHECK(not requires { typename TestType::difference_type; });
}

TEMPLATE_PRODUCT_TEST_CASE (
    "exfs::iterator::incrementable_traits - pointers",
    "[unit][std-parity][iterator]",
    (std::incrementable_traits, exfs::iterator::incrementable_traits),
    (Empty*, int*, Empty**, Empty* const, int* const, Empty const*)
) {
    CHECK(std::is_same_v<typename TestType::difference_type, std::ptrdiff_t>);
}

struct Member_Type { struct difference_type; };

TEMPLATE_PRODUCT_TEST_CASE (
    "exfs::iterator::incrementable_traits - with member difference_type",
    "[unit][std-parity][iterator]",
    (std::incrementable_traits, exfs::iterator::incrementable_traits),
    (Member_Type, Member_Type const)
) {
    CHECK(std::is_same_v<
        typename TestType::difference_type,
        Member_Type::difference_type
    >);
}

///
/// struct indirectly_readable_traits
///

TEMPLATE_PRODUCT_TEST_CASE (
    "exfs::iterator::indirectly_readable_traits - primary template",
    "[unit][std-parity][iterator]",
    (std::indirectly_readable_traits, exfs::iterator::indirectly_readable_traits),
    (Empty, Empty const)
) {
    CHECK(not requires { typename TestType::value_type; });
}

TEMPLATE_PRODUCT_TEST_CASE (
    "exfs::iterator::indirectly_readable_traits - pointers",
    "[unit][std-parity][iterator]",
    (std::indirectly_readable_traits, exfs::iterator::indirectly_readable_traits),
    (
        Empty*,
        Empty const*,
        Empty volatile*,
        Empty const volatile*,
        Empty* const,
        Empty const* const,
        Empty volatile* const,
        Empty const volatile* const
    )
) {
    CHECK(std::is_same_v<typename TestType::value_type, Empty>);
}

TEMPLATE_PRODUCT_TEST_CASE (
    "exfs::iterator::indirectly_readable_traits - arrays",
    "[unit][std-parity][iterator]",
    (std::indirectly_readable_traits, exfs::iterator::indirectly_readable_traits),
    (
        Empty[1],
        Empty const[2],
        Empty volatile[3],
        Empty const volatile[4]
    )
) {
    CHECK(std::is_same_v<typename TestType::value_type, Empty>);
}

struct Target;
template <typename Val>
struct Value_Type_Member { using value_type = Val; };

TEMPLATE_PRODUCT_TEST_CASE (
    "exfs::iterator::indirectly_readable_traits - value_type member",
    "[unit][std-parity][iterator]",
    (std::indirectly_readable_traits, exfs::iterator::indirectly_readable_traits),
    (
        Value_Type_Member<Target>,
        Value_Type_Member<Target const>,
        Value_Type_Member<Target volatile>,
        Value_Type_Member<Target const volatile>,
        Value_Type_Member<Target> const,
        Value_Type_Member<Target const> const,
        Value_Type_Member<Target volatile> const,
        Value_Type_Member<Target const volatile> const
    )
) {
    CHECK(std::is_same_v<typename TestType::value_type, Target>);
}

TEMPLATE_PRODUCT_TEST_CASE (
    "exfs::iterator::indirectly_readable_traits - value_type member non-object",
    "[unit][std-parity][iterator]",
    (std::indirectly_readable_traits, exfs::iterator::indirectly_readable_traits),
    (
        Value_Type_Member<Target()>,
        Value_Type_Member<Target&>,
        Value_Type_Member<void>,
        Value_Type_Member<Target()> const,
        Value_Type_Member<Target&> const,
        Value_Type_Member<void> const
    )
) {
    CHECK(not requires { typename TestType::value_type; });
}

template <typename Elem>
struct Element_Type_Member { using element_type = Elem; };

TEMPLATE_PRODUCT_TEST_CASE (
    "exfs::iterator::indirectly_readable_traits - element_type member",
    "[unit][std-parity][iterator]",
    (std::indirectly_readable_traits, exfs::iterator::indirectly_readable_traits),
    (
        Element_Type_Member<Target>,
        Element_Type_Member<Target const>,
        Element_Type_Member<Target volatile>,
        Element_Type_Member<Target const volatile>,
        Element_Type_Member<Target> const,
        Element_Type_Member<Target const> const,
        Element_Type_Member<Target volatile> const,
        Element_Type_Member<Target const volatile> const
    )
) {
    CHECK(std::is_same_v<typename TestType::value_type, Target>);
}

TEMPLATE_PRODUCT_TEST_CASE (
    "exfs::iterator::indirectly_readable_traits - element_type member non-object",
    "[unit][std-parity][iterator]",
    (std::indirectly_readable_traits, exfs::iterator::indirectly_readable_traits),
    (
        Element_Type_Member<Target()>,
        Element_Type_Member<Target&>,
        Element_Type_Member<void>,
        Element_Type_Member<Target()> const,
        Element_Type_Member<Target&> const,
        Element_Type_Member<void> const
    )
) {
    CHECK(not requires { typename TestType::value_type; });
}

template <typename Val, typename Elem>
struct Value_Element_Members {
    using value_type = Val;
    using element_type = Elem;
};

TEMPLATE_PRODUCT_TEST_CASE (
    "exfs::iterator::indirectly_readable_traits - value_type and element_type members",
    "[unit][std-parity][iterator]",
    (std::indirectly_readable_traits, exfs::iterator::indirectly_readable_traits),
    (
        (Value_Element_Members<Target, Target>),
        (Value_Element_Members<Target, Target const>),
        (Value_Element_Members<Target, Target volatile>),
        (Value_Element_Members<Target, Target const volatile>),
        (Value_Element_Members<Target const, Target>),
        (Value_Element_Members<Target const, Target const>),
        (Value_Element_Members<Target const, Target volatile>),
        (Value_Element_Members<Target const, Target const volatile>),
        (Value_Element_Members<Target volatile, Target>),
        (Value_Element_Members<Target volatile, Target const>),
        (Value_Element_Members<Target volatile, Target volatile>),
        (Value_Element_Members<Target volatile, Target const volatile>),
        (Value_Element_Members<Target const volatile, Target>),
        (Value_Element_Members<Target const volatile, Target const>),
        (Value_Element_Members<Target const volatile, Target volatile>),
        (Value_Element_Members<Target const volatile, Target const volatile>),
        (Value_Element_Members<Target, Target> const),
        (Value_Element_Members<Target, Target const> const),
        (Value_Element_Members<Target, Target volatile> const),
        (Value_Element_Members<Target, Target const volatile> const),
        (Value_Element_Members<Target const, Target> const),
        (Value_Element_Members<Target const, Target const> const),
        (Value_Element_Members<Target const, Target volatile> const),
        (Value_Element_Members<Target const, Target const volatile> const),
        (Value_Element_Members<Target volatile, Target> const),
        (Value_Element_Members<Target volatile, Target const> const),
        (Value_Element_Members<Target volatile, Target volatile> const),
        (Value_Element_Members<Target volatile, Target const volatile> const),
        (Value_Element_Members<Target const volatile, Target> const),
        (Value_Element_Members<Target const volatile, Target const> const),
        (Value_Element_Members<Target const volatile, Target volatile> const),
        (Value_Element_Members<Target const volatile, Target const volatile> const)
    )
) {
    CHECK(std::is_same_v<typename TestType::value_type, Target>);
}

TEMPLATE_PRODUCT_TEST_CASE (
    "exfs::iterator::indirectly_readable_traits - value_type and element_type members non-object",
    "[unit][std-parity][iterator]",
    (std::indirectly_readable_traits, exfs::iterator::indirectly_readable_traits),
    (
        (Value_Element_Members<Target, Target()>),
        (Value_Element_Members<Target, Target&>),
        (Value_Element_Members<Target, void>),
        (Value_Element_Members<Target, Target()> const),
        (Value_Element_Members<Target, Target&> const),
        (Value_Element_Members<Target, void> const),
        (Value_Element_Members<Target(), Target>),
        (Value_Element_Members<Target&, Target>),
        (Value_Element_Members<void, Target>),
        (Value_Element_Members<Target(), Target> const),
        (Value_Element_Members<Target&, Target> const),
        (Value_Element_Members<void, Target> const)
    )
) {
    CHECK(not requires { typename TestType::value_type; });
}

TEMPLATE_PRODUCT_TEST_CASE (
    "exfs::iterator::indirectly_readable_traits - value_type and element_type members mismatch",
    "[unit][std-parity][iterator]",
    (std::indirectly_readable_traits, exfs::iterator::indirectly_readable_traits),
    (
        (Value_Element_Members<Target, Empty>),
        (Value_Element_Members<Target, Empty const>),
        (Value_Element_Members<Target const, Empty>),
        (Value_Element_Members<Target const, Empty const>),
        (Value_Element_Members<Empty, Target>),
        (Value_Element_Members<Empty, Target const>),
        (Value_Element_Members<Empty const, Target>),
        (Value_Element_Members<Empty const, Target const>),
        (Value_Element_Members<Target, Empty> const),
        (Value_Element_Members<Target, Empty const> const),
        (Value_Element_Members<Target const, Empty> const),
        (Value_Element_Members<Target const, Empty const> const),
        (Value_Element_Members<Empty, Target> const),
        (Value_Element_Members<Empty, Target const> const),
        (Value_Element_Members<Empty const, Target> const),
        (Value_Element_Members<Empty const, Target const> const)
    )
) {
    CHECK(not requires { typename TestType::value_type; });
}

///
/// aliases
///

using Alias_Test_Types = std::tuple<
    int*,
    int const*,
    int volatile*,
    int const volatile*,
    int* const,
    int const* const,
    int volatile* const,
    int const volatile* const,

    int* const&,
    int const*&,

    typename std::vector<int>::iterator,
    typename std::vector<int>::const_iterator,
    typename std::list<int>::iterator,
    typename std::list<int>::const_iterator,
    typename std::map<int, int>::iterator,
    typename std::map<int, int>::const_iterator,
    std::istream_iterator<int>,

    typename std::vector<int>::iterator&,
    typename std::list<int>::iterator const&
>;

#define CHECK_ALIAS_PARITY(ALIAS_NAME, MODEL)                                  \
CHECK((std::is_same_v<std::ALIAS_NAME<MODEL>, exfs::iterator::ALIAS_NAME<MODEL>>));

TEMPLATE_LIST_TEST_CASE (
    "exfs::iterator::iter_value_t",
    "[unit][std-parity][iterator]",
    Alias_Test_Types
) {
    CHECK_ALIAS_PARITY(iter_value_t, TestType);
}

TEMPLATE_LIST_TEST_CASE (
    "exfs::iterator::iter_reference_t",
    "[unit][std-parity][iterator]",
    Alias_Test_Types
) {
    CHECK_ALIAS_PARITY(iter_reference_t, TestType);
}

struct Member_Ref {
    struct reference;
};

struct Member_Ref_Derefable {
    struct reference;
    struct other_ref;

    other_ref& operator * ();
};

struct Derefable {
    struct other_ref;

    other_ref& operator * ();
};

TEST_CASE (
    "exfs::iterator::iter_reference_t - focus tests",
    "[unit][iterator]"
) {
    using exfs::iterator::iter_reference_t;

    CHECK(std::is_same_v<
        iter_reference_t<Member_Ref>,
        Member_Ref::reference
    >);

    CHECK(std::is_same_v<
        iter_reference_t<Member_Ref_Derefable>,
        Member_Ref_Derefable::reference
    >);

    CHECK(std::is_same_v<
        iter_reference_t<Derefable>,
        Derefable::other_ref&
    >);
}

TEST_CASE (
    "exfs::iterator::iter_pointer_t",
    "[unit][iterator]"
) {
    using exfs::iterator::iter_pointer_t;

    #define CHECK_ITER_PTR(QUERY_TYPE, EXPECTED_TYPE)                          \
    CHECK((std::is_same_v<iter_pointer_t<QUERY_TYPE>, EXPECTED_TYPE>));

    CHECK_ITER_PTR(int*, int*);
    CHECK_ITER_PTR(int const*, int const*);
    CHECK_ITER_PTR(int volatile*, int volatile*);
    CHECK_ITER_PTR(int const volatile*, int const volatile*);
    CHECK_ITER_PTR(int* const, int*);
    CHECK_ITER_PTR(int const* const, int const*);
    CHECK_ITER_PTR(int volatile* const, int volatile*);
    CHECK_ITER_PTR(int const volatile* const, int const volatile*);

    CHECK_ITER_PTR(int const*&, int const*);

    CHECK_ITER_PTR(typename std::vector<int>::iterator, int*);
    CHECK_ITER_PTR(typename std::vector<int>::const_iterator, int const*);
    CHECK_ITER_PTR(typename std::list<int>::iterator, int*);
    CHECK_ITER_PTR(typename std::list<int>::const_iterator, int const*);
    CHECK((std::is_same_v<
        iter_pointer_t<typename std::map<int, int>::iterator>,
        std::pair<int const, int>*
    >));
    CHECK((std::is_same_v<
        iter_pointer_t<typename std::map<int, int>::const_iterator>,
        std::pair<int const, int> const*
    >));
    CHECK_ITER_PTR(std::istream_iterator<int>, int const*);

    CHECK_ITER_PTR(std::unique_ptr<int>, int*);
    CHECK_ITER_PTR(std::unique_ptr<int> const, int*);
    CHECK_ITER_PTR(std::unique_ptr<int const>, int const*);

    #undef CHECK_ITER_PTR
}

TEMPLATE_LIST_TEST_CASE (
    "exfs::iterator::iter_difference_t",
    "[unit][std-parity][iterator]",
    Alias_Test_Types
) {
    CHECK_ALIAS_PARITY(iter_difference_t, TestType);
}

TEMPLATE_LIST_TEST_CASE (
    "exfs::iterator::iter_rvalue_reference_t",
    "[unit][std-parity][iterator]",
    Alias_Test_Types
) {
    CHECK_ALIAS_PARITY(iter_rvalue_reference_t, TestType);
}

TEMPLATE_LIST_TEST_CASE (
// TEST_CASE (
    "exfs::iterator::iter_common_reference_t",
    "[unit][std-parity][iterator]",
    Alias_Test_Types
) {
    CHECK_ALIAS_PARITY(iter_common_reference_t, TestType);
}

#undef CHECK_ALIAS_PARITY

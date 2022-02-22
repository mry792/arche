#include "exfs/iterator/traits.hpp"

#include <cstddef>
#include <iterator>
#include <type_traits>

#include <catch2/catch.hpp>

struct Empty {};

///
/// struct incrementable_traits
///

TEMPLATE_PRODUCT_TEST_CASE(
    "exfs::iterator::incrementable_traits - primary template",
    "[unit][std-parity][iterator]",
    (std::incrementable_traits, exfs::iterator::incrementable_traits),
    (Empty, Empty const)
) {
    CHECK(not requires { typename TestType::difference_type; });
}

TEMPLATE_PRODUCT_TEST_CASE(
    "exfs::iterator::incrementable_traits - pointers",
    "[unit][std-parity][iterator]",
    (std::incrementable_traits, exfs::iterator::incrementable_traits),
    (Empty*, int*, Empty**, Empty* const, int* const, Empty const*)
) {
    CHECK(std::is_same_v<typename TestType::difference_type, std::ptrdiff_t>);
}

struct Member_Type { struct difference_type; };

TEMPLATE_PRODUCT_TEST_CASE(
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

TEMPLATE_PRODUCT_TEST_CASE(
    "exfs::iterator::indirectly_readable_traits - primary template",
    "[unit][std-parity][iterator]",
    (std::indirectly_readable_traits, exfs::iterator::indirectly_readable_traits),
    (Empty, Empty const)
) {
    CHECK(not requires { typename TestType::value_type; });
}

TEMPLATE_PRODUCT_TEST_CASE(
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

TEMPLATE_PRODUCT_TEST_CASE(
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

TEMPLATE_PRODUCT_TEST_CASE(
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

TEMPLATE_PRODUCT_TEST_CASE(
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

TEMPLATE_PRODUCT_TEST_CASE(
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

TEMPLATE_PRODUCT_TEST_CASE(
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

TEMPLATE_PRODUCT_TEST_CASE(
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

TEMPLATE_PRODUCT_TEST_CASE(
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

TEMPLATE_PRODUCT_TEST_CASE(
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

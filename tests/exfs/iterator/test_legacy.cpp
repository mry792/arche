#include "exfs/iterator/legacy.hpp"

#include <forward_list>
#include <iterator>
#include <set>
#include <type_traits>
#include <vector>

#include <catch2/catch.hpp>

struct Iter_Model {
    int const& operator * () const;
    Iter_Model& operator ++ ();
    Iter_Model operator ++ (int);
};

// TODO: Define Input_Iter_Model such that it is not also a forward iter.
template <typename Derived>
struct Input_Iter_ {
    using difference_type = std::ptrdiff_t;
    using value_type = int;
    using reference = value_type const&;

    // Input_Iter_ (Input_Iter_ const&) = delete;

    reference operator * () const;
    Derived& operator ++ ();
    Derived operator ++ (int);

    bool operator == (Derived const&) const;
    bool operator != (Derived const&) const;
};
struct Input_Iter_Model : Input_Iter_<Input_Iter_Model> {};

template <typename Derived>
struct Fwd_ : Input_Iter_<Derived> {
    using Parent_ = Input_Iter_<Derived>;
    using typename Parent_::value_type;
    using reference = value_type&;

    // Fwd_ (Fwd_ const&);

    reference operator * ();
};
struct Forward_Iter_Model : Fwd_<Forward_Iter_Model> {};

template <typename Derived>
struct Bidir_ : Fwd_<Derived> {
    Derived& operator -- ();
    Derived operator -- (int);
};
struct Bidirectional_Iter_Model : Bidir_<Bidirectional_Iter_Model> {};

template <typename Derived>
struct Rand_ : Bidir_<Derived> {
    using Parent_ = Bidir_<Derived>;
    using typename Parent_::difference_type;
    using typename Parent_::reference;

    Derived& operator += (difference_type n);
    Derived& operator -= (difference_type n);

    friend bool operator <  (Derived const& a, Derived const& b) { return true; }
    friend bool operator <= (Derived const& a, Derived const& b) { return true; }
    friend bool operator >  (Derived const& a, Derived const& b) { return true; }
    friend bool operator >= (Derived const& a, Derived const& b) { return true; }

    friend Derived operator + (Derived const& iter, difference_type n) { return iter; }
    friend Derived operator + (difference_type n, Derived const& iter) { return iter; }
    friend Derived operator - (Derived const& iter, difference_type n) { return iter; }
    friend difference_type operator - (Derived const& i, Derived const& j) { return 0; }

    reference operator [] (difference_type n);
};
struct Random_Access_Iter_Model : Rand_<Random_Access_Iter_Model> {};

struct Basic_Iterator {
    int operator * ();
    Basic_Iterator& operator ++ ();
    Basic_Iterator operator ++ (int);
};

using Output_Iterator = std::insert_iterator<std::set<int>>;
using Input_Iterator = std::istream_iterator<int>;

using Forward_Iterator = typename std::forward_list<int>::iterator;
using Bidirectional_Iterator = typename std::set<int>::iterator;
using Random_Access_Iterator = typename std::vector<int>::iterator;
using Contiguous_Iterator = int*;

using Forward_Const_Iterator = typename std::forward_list<int>::const_iterator;
using Bidirectional_Const_Iterator = typename std::set<int>::const_iterator;
using Random_Access_Const_Iterator = typename std::vector<int>::const_iterator;
using Contiguous_Const_Iterator = int const*;

///
/// legacy concepts
///

TEST_CASE(
    "exfs::iterator::legacy_iterator",
    "[unit][iterator]"
) {
    using exfs::iterator::legacy_iterator;

    CHECK(legacy_iterator<Iter_Model>);
    CHECK(legacy_iterator<Input_Iter_Model>);
    CHECK(legacy_iterator<Forward_Iter_Model>);
    CHECK(legacy_iterator<Bidirectional_Iter_Model>);
    CHECK(legacy_iterator<Random_Access_Iter_Model>);

    CHECK(not legacy_iterator<int>);
    CHECK(legacy_iterator<Basic_Iterator>);
    CHECK(legacy_iterator<Output_Iterator>);
    CHECK(legacy_iterator<Input_Iterator>);

    CHECK(legacy_iterator<Forward_Iterator>);
    CHECK(legacy_iterator<Bidirectional_Iterator>);
    CHECK(legacy_iterator<Random_Access_Iterator>);
    CHECK(legacy_iterator<Contiguous_Iterator>);

    CHECK(legacy_iterator<Forward_Const_Iterator>);
    CHECK(legacy_iterator<Bidirectional_Const_Iterator>);
    CHECK(legacy_iterator<Random_Access_Const_Iterator>);
    CHECK(legacy_iterator<Contiguous_Const_Iterator>);
}

TEST_CASE(
    "exfs::iterator::legacy_input_iterator",
    "[unit][iterator]"
) {
    using exfs::iterator::legacy_input_iterator;

    CHECK(not legacy_input_iterator<Iter_Model>);
    CHECK(legacy_input_iterator<Input_Iter_Model>);
    CHECK(legacy_input_iterator<Forward_Iter_Model>);
    CHECK(legacy_input_iterator<Bidirectional_Iter_Model>);
    CHECK(legacy_input_iterator<Random_Access_Iter_Model>);

    CHECK(not legacy_input_iterator<int>);
    CHECK(not legacy_input_iterator<Basic_Iterator>);
    CHECK(not legacy_input_iterator<Output_Iterator>);
    CHECK(legacy_input_iterator<Input_Iterator>);

    CHECK(legacy_input_iterator<Forward_Iterator>);
    CHECK(legacy_input_iterator<Bidirectional_Iterator>);
    CHECK(legacy_input_iterator<Random_Access_Iterator>);
    CHECK(legacy_input_iterator<Contiguous_Iterator>);

    CHECK(legacy_input_iterator<Forward_Const_Iterator>);
    CHECK(legacy_input_iterator<Bidirectional_Const_Iterator>);
    CHECK(legacy_input_iterator<Random_Access_Const_Iterator>);
    CHECK(legacy_input_iterator<Contiguous_Const_Iterator>);
}

TEST_CASE(
    "exfs::iterator::legacy_output_iterator",
    "[unit][iterator]"
) {
    using exfs::iterator::legacy_output_iterator;

    CHECK(not legacy_output_iterator<Iter_Model, int>);
    CHECK(not legacy_output_iterator<Input_Iter_Model, int>);
    CHECK(legacy_output_iterator<Forward_Iter_Model, int>);
    CHECK(legacy_output_iterator<Bidirectional_Iter_Model, int>);
    CHECK(legacy_output_iterator<Random_Access_Iter_Model, int>);

    CHECK(not legacy_output_iterator<int, int>);
    CHECK(not legacy_output_iterator<Basic_Iterator, int>);
    CHECK(legacy_output_iterator<Output_Iterator, int>);
    CHECK(not legacy_output_iterator<Input_Iterator, int>);

    // NOTE that a std::set::iterator is not an output iterator.
    CHECK(legacy_output_iterator<Forward_Iterator, int>);
    CHECK(not legacy_output_iterator<Bidirectional_Iterator, int>);
    CHECK(legacy_output_iterator<Random_Access_Iterator, int>);
    CHECK(legacy_output_iterator<Contiguous_Iterator, int>);

    CHECK(not legacy_output_iterator<Forward_Const_Iterator, int>);
    CHECK(not legacy_output_iterator<Bidirectional_Const_Iterator, int>);
    CHECK(not legacy_output_iterator<Random_Access_Const_Iterator, int>);
    CHECK(not legacy_output_iterator<Contiguous_Const_Iterator, int>);
}

TEST_CASE(
    "exfs::iterator::legacy_forward_iterator",
    "[unit][iterator]"
) {
    using exfs::iterator::legacy_forward_iterator;

    CHECK(not legacy_forward_iterator<Iter_Model>);
    // TODO: Define Input_Iter_Model such that it is not also a forward iter.
    // CHECK(not legacy_forward_iterator<Input_Iter_Model>);
    CHECK(legacy_forward_iterator<Forward_Iter_Model>);
    CHECK(legacy_forward_iterator<Bidirectional_Iter_Model>);
    CHECK(legacy_forward_iterator<Random_Access_Iter_Model>);

    // NOTE that std::istream_iterator fits all the requirements of
    // legacy_forward_iterator except the "multipass guarantee" which is not
    // expressible using C++20 concepts.
    CHECK(not legacy_forward_iterator<int>);
    CHECK(not legacy_forward_iterator<Basic_Iterator>);
    CHECK(not legacy_forward_iterator<Output_Iterator>);
    // CHECK(not legacy_forward_iterator<Input_Iterator>);

    CHECK(legacy_forward_iterator<Forward_Iterator>);
    CHECK(legacy_forward_iterator<Bidirectional_Iterator>);
    CHECK(legacy_forward_iterator<Random_Access_Iterator>);
    CHECK(legacy_forward_iterator<Contiguous_Iterator>);

    CHECK(legacy_forward_iterator<Forward_Const_Iterator>);
    CHECK(legacy_forward_iterator<Bidirectional_Const_Iterator>);
    CHECK(legacy_forward_iterator<Random_Access_Const_Iterator>);
    CHECK(legacy_forward_iterator<Contiguous_Const_Iterator>);
}

TEST_CASE(
    "exfs::iterator::legacy_bidirectional_iterator",
    "[unit][iterator]"
) {
    using exfs::iterator::legacy_bidirectional_iterator;

    CHECK(not legacy_bidirectional_iterator<Iter_Model>);
    CHECK(not legacy_bidirectional_iterator<Input_Iter_Model>);
    CHECK(not legacy_bidirectional_iterator<Forward_Iter_Model>);
    CHECK(legacy_bidirectional_iterator<Bidirectional_Iter_Model>);
    CHECK(legacy_bidirectional_iterator<Random_Access_Iter_Model>);

    CHECK(not legacy_bidirectional_iterator<int>);
    CHECK(not legacy_bidirectional_iterator<Basic_Iterator>);
    CHECK(not legacy_bidirectional_iterator<Output_Iterator>);
    CHECK(not legacy_bidirectional_iterator<Input_Iterator>);

    CHECK(not legacy_bidirectional_iterator<Forward_Iterator>);
    CHECK(legacy_bidirectional_iterator<Bidirectional_Iterator>);
    CHECK(legacy_bidirectional_iterator<Random_Access_Iterator>);
    CHECK(legacy_bidirectional_iterator<Contiguous_Iterator>);

    CHECK(not legacy_bidirectional_iterator<Forward_Const_Iterator>);
    CHECK(legacy_bidirectional_iterator<Bidirectional_Const_Iterator>);
    CHECK(legacy_bidirectional_iterator<Random_Access_Const_Iterator>);
    CHECK(legacy_bidirectional_iterator<Contiguous_Const_Iterator>);
}

TEST_CASE(
    "exfs::iterator::legacy_random_access_iterator",
    "[unit][iterator]"
) {
    using exfs::iterator::legacy_random_access_iterator;

    CHECK(not legacy_random_access_iterator<Iter_Model>);
    CHECK(not legacy_random_access_iterator<Input_Iter_Model>);
    CHECK(not legacy_random_access_iterator<Forward_Iter_Model>);
    CHECK(not legacy_random_access_iterator<Bidirectional_Iter_Model>);
    CHECK(legacy_random_access_iterator<Random_Access_Iter_Model>);

    CHECK(not legacy_random_access_iterator<int>);
    CHECK(not legacy_random_access_iterator<Basic_Iterator>);
    CHECK(not legacy_random_access_iterator<Output_Iterator>);
    CHECK(not legacy_random_access_iterator<Input_Iterator>);

    CHECK(not legacy_random_access_iterator<Forward_Iterator>);
    CHECK(not legacy_random_access_iterator<Bidirectional_Iterator>);
    CHECK(legacy_random_access_iterator<Random_Access_Iterator>);
    CHECK(legacy_random_access_iterator<Contiguous_Iterator>);

    CHECK(not legacy_random_access_iterator<Forward_Const_Iterator>);
    CHECK(not legacy_random_access_iterator<Bidirectional_Const_Iterator>);
    CHECK(legacy_random_access_iterator<Random_Access_Const_Iterator>);
    CHECK(legacy_random_access_iterator<Contiguous_Const_Iterator>);
}

///
/// legacy traits
///

TEST_CASE(
    "exfs::iterator::iterator_category",
    "[unit][std-parity][iterator]"
) {
    using exfs::iterator::iterator_category_t;

    #define CHECK_CATEGORY(ITER_TYPE, CATEGORY)                                \
    CHECK((std::is_same_v<                                                     \
        typename std::iterator_traits<ITER_TYPE>::iterator_category,           \
        std::CATEGORY ## _tag                                                  \
    >));                                                                       \
    CHECK((std::is_same_v<                                                     \
        iterator_category_t<ITER_TYPE>,                                        \
        exfs::iterator::CATEGORY ## _tag                                       \
    >));

    // CHECK_CATEGORY(Output_Iterator, );
    // CHECK_CATEGORY(Input_Iterator, input_iterator);
    CHECK_CATEGORY(Forward_Iterator, forward_iterator);
    CHECK_CATEGORY(Bidirectional_Iterator, bidirectional_iterator);
    CHECK_CATEGORY(Random_Access_Iterator, random_access_iterator);
    // CHECK_CATEGORY(Contiguous_Iterator, contiguous_iterator);
    CHECK_CATEGORY(Forward_Const_Iterator, forward_iterator);
    CHECK_CATEGORY(Bidirectional_Const_Iterator, bidirectional_iterator);
    CHECK_CATEGORY(Random_Access_Const_Iterator, random_access_iterator);
    // CHECK_CATEGORY(Contiguous_Const_Iterator, contiguous_iterator);

    #undef CHECK_CATEGORY
}

struct Iter_All_Members {
    struct difference_type;
    struct value_type;
    struct pointer;
    struct reference;
    struct iterator_category;
};

TEMPLATE_TEST_CASE(
    "exfs::iterator::iterator_traits - all members",
    "[unit][std-parity][iterator]",
    std::iterator_traits<Iter_All_Members>,
    exfs::iterator::iterator_traits<Iter_All_Members>
) {
    #define DO_CHECK(TRAIT)                                                    \
    CHECK(std::is_same_v<typename TestType::TRAIT, Iter_All_Members::TRAIT>);

    DO_CHECK(difference_type);
    DO_CHECK(value_type);
    DO_CHECK(pointer);
    DO_CHECK(reference);
    DO_CHECK(iterator_category);

    #undef DO_CHECK
}

struct Iter_Four_Members {
    struct difference_type;
    struct value_type;
    // struct pointer;
    struct reference;
    struct iterator_category;
};

TEMPLATE_TEST_CASE(
    "exfs::iterator::iterator_traits - all members except pointer",
    "[unit][std-parity][iterator]",
    std::iterator_traits<Iter_Four_Members>,
    exfs::iterator::iterator_traits<Iter_Four_Members>
) {
    #define DO_CHECK(TRAIT)                                                    \
    CHECK(std::is_same_v<typename TestType::TRAIT, Iter_Four_Members::TRAIT>);

    DO_CHECK(difference_type);
    DO_CHECK(value_type);
    CHECK(std::is_same_v<typename TestType::pointer, void>);
    DO_CHECK(reference);
    DO_CHECK(iterator_category);

    #undef DO_CHECK
}

// TODO: Test different iterator categories with iterator_traits.

TEST_CASE(
    "exfs::iterator::iterator_traits - pointer specialization",
    "[unit][std-parity][iterator]"
) {
    #define DO_TEST(NAMESPACE, QUALIFIERS)                                     \
    {                                                                          \
        using traits = NAMESPACE::iterator_traits<int QUALIFIERS*>;            \
        CHECK(std::is_same_v<traits::difference_type, std::ptrdiff_t>);        \
        CHECK(std::is_same_v<traits::value_type, int>);                        \
        CHECK(std::is_same_v<traits::pointer, int QUALIFIERS*>);               \
        CHECK(std::is_same_v<traits::reference, int QUALIFIERS&>);             \
        CHECK(std::is_same_v<                                                  \
            traits::iterator_category, NAMESPACE::random_access_iterator_tag>);\
        CHECK(std::is_same_v<                                                  \
                traits::iterator_concept, NAMESPACE::contiguous_iterator_tag>);\
    }

    DO_TEST(std, );
    DO_TEST(std, const);
    DO_TEST(std, volatile);
    DO_TEST(std, const volatile);
    DO_TEST(exfs::iterator, );
    DO_TEST(exfs::iterator, const);
    DO_TEST(exfs::iterator, volatile);
    DO_TEST(exfs::iterator, const volatile);

    #undef DO_TEST
}

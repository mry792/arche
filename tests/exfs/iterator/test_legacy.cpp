#include "exfs/iterator/legacy.hpp"

#include <forward_list>
#include <iterator>
#include <set>
#include <type_traits>
#include <vector>

#include <catch2/catch.hpp>

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
using Contiguous_Iterator = Random_Access_Iterator;

using Forward_Const_Iterator = typename std::forward_list<int>::const_iterator;
using Bidirectional_Const_Iterator = typename std::set<int>::const_iterator;
using Random_Access_Const_Iterator = typename std::vector<int>::const_iterator;
using Contiguous_Const_Iterator = Random_Access_Const_Iterator;

TEST_CASE(
    "exfs::iterator::legacy_iterator",
    "[unit][iterator]"
) {
    using exfs::iterator::legacy_iterator;

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

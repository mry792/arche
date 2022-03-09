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

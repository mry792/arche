#ifndef EXFS_ITERATOR_CATEGORY_TAGS_HPP_
#define EXFS_ITERATOR_CATEGORY_TAGS_HPP_

/**
 * Iterator category tags carry information that can be used to select the most
 * efficient algorithms for the specific requirement set that is implied by the
 * category.
 */

#if __STDC_HOSTED__

#include <iterator>

#else  // __STDC_HOSTED__

namespace std {
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};
struct contiguous_iterator_tag : public random_access_iterator_tag {};
}  // namespace std

#endif  // __STDC_HOSTED__

/**
 * With a hosted implementation, it's essential that the category tags be the
 * same types as are defined by the standard library. Otherwise the concepts
 * defined in exfs won't work correctly with types that use the std category
 * tags.
 */
namespace exfs::iterator {
using input_iterator_tag = std::input_iterator_tag;
using output_iterator_tag = std::output_iterator_tag;
using forward_iterator_tag = std::forward_iterator_tag;
using bidirectional_iterator_tag = std::bidirectional_iterator_tag;
using random_access_iterator_tag = std::random_access_iterator_tag;
using contiguous_iterator_tag = std::contiguous_iterator_tag;
}  // exfs::iterator

#endif  // EXFS_ITERATOR_CATEGORY_TAGS_HPP_

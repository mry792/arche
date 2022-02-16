#ifndef EXFS_ITERATOR_CATEGORY_TAGS_HPP_
#define EXFS_ITERATOR_CATEGORY_TAGS_HPP_

namespace exfs::iterator {
/*!
 * Iterator category tags carry information that can be used to select the most
 * efficient algorithms for the specific requirement set that is implied by the
 * category.
 */
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};
struct contiguous_iterator_tag : public random_access_iterator_tag {};
}  // namespace exfs::iterator

#endif  // EXFS_ITERATOR_CATEGORY_TAGS_HPP_

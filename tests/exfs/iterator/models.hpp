#ifndef TESTS_EXFS_ITERATOR_MODEL_CONCEPTS_HPP_
#define TESTS_EXFS_ITERATOR_MODEL_CONCEPTS_HPP_

#include <cstddef>

#include <compare>

#include "exfs/iterator/category_tags.hpp"
#include "exfs/iterator/concepts.hpp"

namespace exfs::iterator {
namespace models {
struct Value {};

namespace __detail {
template <typename Concrete>
struct __in_or_out_iter {
    using difference_type = std::ptrdiff_t;

    Concrete& operator ++ ();
    Concrete operator ++ (int);

    Value operator * ();
};

template <typename Concrete>
struct __input_iter : __in_or_out_iter<Concrete> {
    using value_type = Value;
    using reference = value_type&;
    using iterator_category = input_iterator_tag;
    using iterator_concept = input_iterator_tag;

    reference operator * () const;
};

template <typename Concrete>
struct __output_iter : __in_or_out_iter<Concrete> {
    Value& operator * () const;
};

template <typename Concrete>
struct __forward_iter : __input_iter<Concrete> {
    using iterator_category = forward_iterator_tag;
    using iterator_concept = forward_iterator_tag;

    friend bool operator == (Concrete const&, Concrete const&) { return false; }
    friend bool operator != (Concrete const&, Concrete const&) { return true; }
};

template <typename Concrete>
struct __bidir_iter : __forward_iter<Concrete> {
    using iterator_category = bidirectional_iterator_tag;
    using iterator_concept = bidirectional_iterator_tag;

    Concrete& operator -- ();
    Concrete operator -- (int);
};

template <typename Concrete>
struct __rand_acc_iter : __bidir_iter<Concrete> {
  private:
    using Parent_ = __bidir_iter<Concrete>;

  public:
    using reference         = Parent_::reference;
    using difference_type   = Parent_::difference_type;
    using iterator_category = random_access_iterator_tag;
    using iterator_concept  = random_access_iterator_tag;

    friend std::weak_ordering operator <=> (Concrete const&, Concrete const&) {
        return std::weak_ordering::equivalent;
    }

    Concrete& operator += (difference_type);
    Concrete& operator -= (difference_type);
    friend Concrete operator + (Concrete const&, difference_type) { return {}; }
    friend Concrete operator + (difference_type, Concrete const&) { return {}; }
    friend Concrete operator - (Concrete const&, difference_type) { return {}; }
    friend difference_type operator - (Concrete const&, Concrete const&) {
        return {};
    }

    reference operator [] (difference_type) const;
};
}  // namespace __detail

struct input_or_output_iterator
      : __detail::__in_or_out_iter<input_or_output_iterator> {};
struct input_iterator : __detail::__input_iter<input_iterator> {};
struct output_iterator : __detail::__output_iter<output_iterator> {};
struct forward_iterator : __detail::__forward_iter<forward_iterator> {};
struct bidirectional_iterator
      : __detail::__bidir_iter<bidirectional_iterator> {};
struct random_access_iterator
      : __detail::__rand_acc_iter<random_access_iterator> {};
}  // namespace models

static_assert(input_or_output_iterator<models::input_or_output_iterator>);
static_assert(input_iterator<models::input_iterator>);
static_assert(output_iterator<models::output_iterator, models::Value>);
static_assert(forward_iterator<models::forward_iterator>);
static_assert(bidirectional_iterator<models::bidirectional_iterator>);
static_assert(random_access_iterator<models::random_access_iterator>);
}  // exfs::iterator

#endif  // TESTS_EXFS_ITERATOR_MODEL_CONCEPTS_HPP_

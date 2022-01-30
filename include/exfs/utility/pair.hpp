#ifndef EXFS_UTILITY_PAIR_HPP_
#define EXFS_UTILITY_PAIR_HPP_

namespace exfs::utility {
/*!
    A @c pair is a class template that provides a way to store two heterogeneous
    objects as a single unit. A pair is a specific case of a @c tuple with two
    elements.
 */
template <typename T1, typename T2>
struct pair {
    using first_type = T1;
    using second_type = T2;
};
}  // namespace exfs::utility

#endif  // EXFS_UTILITY_PAIR_HPP_

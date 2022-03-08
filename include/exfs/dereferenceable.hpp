#ifndef EXFS_DEREFERENCEABLE_HPP_
#define EXFS_DEREFERENCEABLE_HPP_

#include "exfs/referenceable.hpp"

namespace exfs {
/**
 * Indicates if @p T can be dereferenced.
 */
template <typename T>
concept dereferenceable = requires (T& t) {
    { *t } -> referenceable;
};
}  // exfs

#endif  // EXFS_DEREFERENCEABLE_HPP_

#ifndef EXFS_REFERENCEABLE_HPP_
#define EXFS_REFERENCEABLE_HPP_

namespace exfs {
namespace __detail {
template <typename T>
using __with_ref = T&;
}  // __detail

/**
 * Indicates if @p T can be referenced (in particular, not @c void).
 */
template <typename T>
concept referenceable = requires {
    typename __detail::__with_ref<T>;
};
}  // exfs

#endif  // EXFS_REFERENCEABLE_HPP_

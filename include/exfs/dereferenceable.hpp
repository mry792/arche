#ifndef EXFS_DEREFERENCEABLE_HPP_
#define EXFS_DEREFERENCEABLE_HPP_

namespace exfs {
namespace __detail {
template <typename T>
using __with_ref = T&;

template <typename T>
concept __can_reference = requires {
    typename __with_ref<T>;
};
}  // __detail

/**
 * Indicates if @p T can be dereferenced.
 */
template <typename T>
concept dereferenceable = requires (T& t) {
    { *t } -> __detail::__can_reference;
};
}  // exfs

#endif  // EXFS_DEREFERENCEABLE_HPP_

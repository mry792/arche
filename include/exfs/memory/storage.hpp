#ifndef EXFS_MEMORY_STORAGE_HPP_
#define EXFS_MEMORY_STORAGE_HPP_

#include <cstddef>

namespace exfs::memory {
/**
 * A type suitable for use as uninitialized storage for an object of type @p T.
 *
 * The @c storage class is a trivial standard-layout type and can be used to
 * create uninitialized memory blocks suitable to hold the objects of the given
 * type. Like other uninitialized storage utilities, the objects must be
 * constructed and destructed explicitly. However, unlike the utilities of the
 * standard library, this @c storage class provides type-restricted member
 * functions for those operations.
 *
 * @warning Unlike @c std::optional, this class does not keep track of whether
 *     the wrapped object has been initialized or not.
 *
 * @tparam T The type to create storage for.
 */
template <typename T>
class storage {
  private:
    alignas(T) std::byte data_[sizeof(T)];
};
}  // exfs::memory

#endif  // EXFS_MEMORY_STORAGE_HPP_

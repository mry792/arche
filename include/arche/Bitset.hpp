#ifndef ARCHE_BITSET_HPP_
#define ARCHE_BITSET_HPP_

#include <concepts>

namespace arche {
/*!
    A class representing a group of individually manipulable bits.

    The primary purpose of this class is to provide a high level abstraction
    with compiler-enforced semantics which can easily be inlined and heavily
    optimized by a compiler. This is to be used as a building block for even
    higher level abstractions of hardware ports and pins while assigning them
    meaningful semantics appropriate for the application.

    This is similar to the @c std::bitset class template from the C++ standard
    library but shifts more of the work to compile-time. Where possible, naming
    is kept consistent with @c std::bitset.

    @tparam V_Bit_Count The number of bits available in the @c Bitset.
    @tparam T_Underlying The underlying value type. Must have at least @p
        V_Bit_Count number of bits.
 */
template <
    std::unsigned_integral auto V_Bit_Count,
    std::unsigned_integral T_Underlying
>
class Bitset {
    static_assert(
        V_Bit_Count <= sizeof(T_Underlying) * 8,
        "T_Underlying must have at least V_Bit_Count bits."
    );

  public:
    /*!
        The type of the underlying value.
     */
    using Underlying = T_Underlying;

    /*!
        @name Constructors
        @{
     */

    /*!
        Construct a @c Bitset with all bits initialized to zero.
     */
    constexpr Bitset () noexcept = default;

    /*!
        @}
     */

    /*!
        @name Access
        @{
     */

    /*!
        Read-only access to the underlying value.
     */
    constexpr Underlying value () const {
        return value_;
    }

    /*!
        @}
     */

  private:
    Underlying value_ = 0u;
};
}  // namespace arche

#endif  // ARCHE_BITSET_HPP_

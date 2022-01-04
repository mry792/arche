#ifndef ARCHE_REGISTER_HPP_
#define ARCHE_REGISTER_HPP_

#include <concepts>

#include "arche/Bitset.hpp"

namespace arche {
/*!
    This class wraps a memory-addressed register of an MCU.

    @tparam V_Address The address of the register as an unsigned integral. This
        is left as an unconstrained value parameter to facilitate testing where
        it can be assigned a reference to a variable serving as a mock MCU
        register.

    @tparam T_Underlying The underyling value type. Must be some kind of
        integral constant

    @tparam V_Bit_Count The number of bits of the register that are available
        for use by the programmer.
 */
template <
    auto V_Address,
    std::unsigned_integral T_Underlying,
    unsigned int V_Bit_Count = sizeof(T_Underlying) * 8
>
class Register {
  public:
    /*!
        The type of the underlying value of the register.
     */
    using Underlying = T_Underlying;

    /*!
        A @c Bitset type corresponding to the register value type.
     */
    using Bitset = arche::Bitset<V_Bit_Count, T_Underlying>;

    /*!
        Set the register to the specified @p value.
        @param[in] value The new value for the register.
     */
    static void set (Bitset value) {
        reg_() = value.value();
    }

    /*!
        Read the value of the register.
     */
    static Bitset get () {
        return Bitset{reg_()};
    }

    /*!
        Set the bits specified in @p mask.
        @param[in] mask The bits set in this bitset will be set in the register.
            Cleared bits in @p mask will be ignored.
     */
    static void set_bits (Bitset mask) {
        reg_() = reg_() | mask.value();
    }

    /*!
        Clears the bits specified in @p mask.
        @param[in] mask The bits set in this bitset will be cleared in the
            register. Cleared bits in @p mask will be ignored.
     */
    static void clear_bits (Bitset mask) {
        reg_() = reg_() & ~mask.value();
    }

  private:
    /*!
        Implementation helper to access the underlying register as a volatile
        reference.
     */
    static Underlying volatile& reg_ () {
        return *reinterpret_cast<Underlying volatile*>(V_Address);
    }
};
}  // namespace arche

#endif  // ARCHE_REGISTER_HPP_

#ifndef ARCHE_BITSET_HPP_
#define ARCHE_BITSET_HPP_

#include <concepts>

namespace arche {
/**
 * A class representing a group of individually manipulable bits.
 *
 * The primary purpose of this class is to provide a high level abstraction with
 * compiler-enforced semantics which can easily be inlined and heavily
 * optimized by a compiler. This is to be used as a building block for even
 * higher level abstractions of hardware ports and pins while assigning them
 * meaningful semantics appropriate for the application.
 *
 * This is similar to the @c std::bitset class template from the C++ standard
 * library but shifts more of the work to compile-time. Where possible, naming
 * is kept consistent with @c std::bitset.
 *
 * @tparam V_Bit_Count The number of bits available in the @c Bitset.
 * @tparam T_Underlying The underlying value type. Must have at least @p
 *     V_Bit_Count number of bits.
 */
template <
    unsigned int V_Bit_Count,
    std::unsigned_integral T_Underlying
>
class Bitset {
    static_assert(
        V_Bit_Count <= sizeof(T_Underlying) * 8,
        "T_Underlying must have at least V_Bit_Count bits."
    );

  public:
    /**
     * The type of the underlying value.
     */
    using Underlying = T_Underlying;

    /**
     * The number of bits of the underlying type used by this @c Bitset.
     */
    static constexpr auto bit_count = V_Bit_Count;

    /**
     * Indicates if every bit of the underlying type is used by this @c Bitset.
     */
    static constexpr bool is_saturated = bit_count == sizeof(Underlying) * 8;

    /**
     * Get a bitmask of the underlying type identifying the bits which are used
     * by this @c Bitset.
     */
    static constexpr Underlying used_bits_mask () {
        if constexpr (is_saturated) {
            return ~Underlying{};
        } else {
            return (Underlying{1} << V_Bit_Count) - 1;
        }
    }

    /**
     * @name Constructors
     * @{
     */

    /**
     * Construct a @c Bitset with all bits initialized to zero.
     */
    constexpr Bitset () noexcept = default;

    /**
     * @c Bitset supports standard copy semantics.
     */
    constexpr Bitset (Bitset const&) noexcept = default;
    constexpr Bitset& operator = (Bitset const&) noexcept = default;

    /**
     * Construct a @c Bitset from the bits of @p value that are set. All other
     * bits, including those outside the bounds of this @c Bitset, are
     * initialized to zero.
     */
    explicit constexpr Bitset (Underlying value)
          : value_{static_cast<Underlying>(value & used_bits_mask())} {}

    // Block implicit conversions to `Underlying` when used to construct a
    // Bitset.
    template <typename T>
    Bitset (T _) = delete;

    /**
     * Create a new @c Bitset with the specified bit set and all others unset.
     *
     * @tparam bit_index The index of the bit to set.
     */
    template <unsigned bit_index>
    static constexpr Bitset bit () {
        static_assert(
            bit_index < V_Bit_Count,
            "Bit index must be within the specified size of the bitset."
        );

        return Bitset{static_cast<Underlying>(1 << bit_index)};
    }

    /**
     * @}
     */

    /**
     * @name Access
     * @{
     */

    /**
     * Read-only access to the underlying value.
     */
    constexpr Underlying value () const {
        return value_;
    }

    /**
     * @}
     */

  private:
    Underlying value_ = 0u;
};
}  // namespace arche

#endif  // ARCHE_BITSET_HPP_

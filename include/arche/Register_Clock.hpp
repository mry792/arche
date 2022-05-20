#ifndef ARCHE_REGISTER_CLOCK_HPP_
#define ARCHE_REGISTER_CLOCK_HPP_

#include <concepts>

#include "arche/concepts/Register.hpp"

namespace arche {
/**
 * Class @c Register_Clock represents a monotonic clock extending a hardware
 * register clock.
 *
 * This class template is a software extension of a hardware timer and should
 * be aliased for a given platform with with all necessary configuration.
 *
 * @tparam T_Register The underlying register which is automatically ticked by
 *     the MCU.
 *
 * @tparam T_Rep Arithmetic type used to represent the number of ticks in the
 *     clock's duration.
 */
template <concepts::Register T_Register, std::unsigned_integral T_Rep>
class Register_Clock {
  public:
    /**
     * The underlying MCU register tracking individual ticks.
     */
    using Register = T_Register;

    /**
     * Type used for the count of the number of ticks.
     */
    using Rep = T_Rep;

    /**
     * Get the current time point.
     */
    Rep now () {
        return high_values_ + static_cast<Rep>(Register::get().value());
    }

    /**
     * Increment the software extension bits.
     *
     * This should be called in an ISR whenever the hardware clock overflows.
     */
    void on_register_overflow () {
        constexpr Rep max_register_value{Register::Bitset::used_bits_mask()};
        high_values_ += max_register_value + 1;
    }

  private:
    Rep high_values_ = 0u;
};
}  // namespace arche

#endif  // ARCHE_REGISTER_CLOCK_HPP_

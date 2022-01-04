#ifndef ARCHE_REGISTER_CLOCK_HPP_
#define ARCHE_REGISTER_CLOCK_HPP_

#include <concepts>

#include "arche/concepts/Register.hpp"

namespace arche {
/*!
    Class @c Register_Clock represents a monotonic clock extending a hardware
    register clock.

    This class template is a software extension of a hardware timer and should
    be aliased for a given platform with with all necessary configuration.

    @tparam T_Register The underlying register which is automatically ticked by
        the MCU.

    @tparam T_Rep Arithmetic type used to represent the number of ticks in the
        clock's duration.
 */
template <concepts::Register T_Register, std::unsigned_integral T_Rep>
class Register_Clock {
  public:
    /*!
        The underlying MCU register tracking the smallest ticks.
     */
    using Register = T_Register;

    /*!
        Type used for the count of the number of ticks.
     */
    using Rep = T_Rep;
};
}  // namespace arche

#endif  // ARCHE_REGISTER_CLOCK_HPP_

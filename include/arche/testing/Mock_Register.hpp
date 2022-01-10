#ifndef ARCHE_TESTING_MOCK_REGISTER_HPP_
#define ARCHE_TESTING_MOCK_REGISTER_HPP_

#include <cstdint>

#include "arche/Register.hpp"

namespace arche::testing {
static std::uint16_t mock_register_value = 0u;

using Mock_Register = arche::Register<
    &mock_register_value,
    std::uint16_t
>;
}  // namespace arche::testing

#endif  // ARCHE_TESTING_MOCK_REGISTER_HPP_

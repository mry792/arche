#ifndef ARCHE_INT_LITERALS_HPP_
#define ARCHE_INT_LITERALS_HPP_

#include <cstdint>

namespace arche::int_literals {
#define INT_LITERAL(TYPE, SUFFIX) \
inline TYPE operator "" SUFFIX(unsigned long long int value) { \
    return static_cast<TYPE>(value); \
}

INT_LITERAL(std::int8_t, _i8)
INT_LITERAL(std::int16_t, _i16)
INT_LITERAL(std::int32_t, _i32)
INT_LITERAL(std::int64_t, _i64)

INT_LITERAL(std::uint8_t, _u8)
INT_LITERAL(std::uint16_t, _u16)
INT_LITERAL(std::uint32_t, _u32)
INT_LITERAL(std::uint64_t, _u64)

#undef INT_LITERAL
}  // namespace arche::int_literals

#endif  // ARCHE_INT_LITERALS_HPP_

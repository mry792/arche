#ifndef ARCHE_CONCEPTS_BITSET_HPP_
#define ARCHE_CONCEPTS_BITSET_HPP_

namespace arche::concepts {
template <typename T>
concept Register = requires (T::Bitset bitset) {
    typename T::Underlying;
    typename T::Bitset;

    T::set(bitset);
    { T::get() } -> std::same_as<typename T::Bitset>;

    T::set_bits(bitset);
    T::clear_bits(bitset);
};
}  // namespace arche::concepts

#endif  // ARCHE_CONCEPTS_BITSET_HPP_

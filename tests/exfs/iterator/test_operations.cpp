#include "exfs/iterator/operations.hpp"

#include <forward_list>
#include <list>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include <catch2/catch.hpp>

namespace {
template <typename T>
struct first_tparam;

template <template <typename...> typename T, typename P1, typename... Ps>
struct first_tparam<T<P1, Ps...>> {
    using type = P1;
};

template <typename T>
using first_tparam_t = typename first_tparam<T>::type;


template <typename Generator>
auto wrap_generator(Generator&& generator) {
    using Catch::Generators::GeneratorWrapper;
    using Catch::Generators::IGenerator;
    using generated_type = first_tparam_t<std::remove_cvref_t<Generator>>;
    using result_type = Catch::Generators::GeneratorWrapper<generated_type>;

    return result_type{std::make_unique<std::remove_cvref_t<Generator>>(
        std::forward<Generator>(generator)
    )};
}
}  // namespace

#define MAKE_WRAPPER(WRAPPER_NAME, WRAPPED_FUNC)                               \
namespace {                                                                    \
struct WRAPPER_NAME {                                                          \
    template <typename... Ts>                                                  \
    constexpr decltype(auto) operator () (Ts&&... ts) const                    \
    noexcept(noexcept(WRAPPED_FUNC(std::forward<Ts>(ts)...))) {                \
        return WRAPPED_FUNC(std::forward<Ts>(ts)...);                          \
    }                                                                          \
};                                                                             \
}

MAKE_WRAPPER(std_advance, std::advance);
MAKE_WRAPPER(exfs_advance, exfs::iterator::advance);

TEMPLATE_TEST_CASE (
    "exfs::iterator::advance",
    "[unit][std-parity][iterator]",
    std_advance,
    exfs_advance
) {
    TestType advance;

    auto do_test = [&] (auto container, auto&& input_data) {
        AND_GIVEN ("an iterator pointing to the 2 element") {
            auto iter = container.begin();
            ++iter;
            ++iter;
            auto const orig_iter = iter;
            CHECK(*iter == 2);

            auto distance = GENERATE_COPY(wrap_generator(input_data));
            auto const distance_str = std::to_string(distance);
            int const expected_value = *orig_iter + distance;
            auto const value_str = std::to_string(expected_value);

            WHEN ("the iterator is advanced " + distance_str + " spaces") {
                advance(iter, distance);

                THEN ("the iterator points to the " + value_str + " element") {
                    CHECK(*iter == expected_value);
                }
            }
        }
    };

    using Catch::Generators::RangeGenerator;

    GIVEN ("a random access container (vector) of 6 elements") {
        do_test(std::vector<int>{{0, 1, 2, 3, 4, 5}}, RangeGenerator(-2, 3));
    }

    GIVEN ("a bidirectional container (list) of 6 elements") {
        do_test(std::list<int>{{0, 1, 2, 3, 4, 5}}, RangeGenerator(-2, 3));
    }

    GIVEN ("a forward container (forward_list) of 6 elements") {
        do_test(
            std::forward_list<int>{{0, 1, 2, 3, 4, 5}},
            RangeGenerator(0, 3)
        );
    }
}

#undef MAKE_WRAPPER

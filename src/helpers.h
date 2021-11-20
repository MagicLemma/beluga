#pragma once
#include <utility>
#include <type_traits>

namespace blga {

template <class Enum>
constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}

constexpr double power_of_12th_root_two(int times) noexcept
{
    constexpr double twelfth_root_two = 1.05946309435929526456182529494634170077920;

    if (times > 0) {
        double product = 1;
        for (std::size_t i = 0; i != times; ++i) {
            product *= twelfth_root_two;
        }
        return product;
    }
    else if (times < 0) {
        double product = 1;
        for (std::size_t i = 0; i != -times; ++i) {
            product /= twelfth_root_two;
        }
        return product;   
    }
    else {
        return 1.0;
    }
}

}
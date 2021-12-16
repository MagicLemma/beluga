#pragma once
#include <cmath>
#include <utility>
#include <type_traits>
#include <ranges>

namespace blga {

inline constexpr auto enumerate(int start = 0)
{
    return std::views::transform([start](auto&& val) mutable {
        return std::make_pair(start++, std::forward<decltype(val)>(val));
    });
}

// key == 0 is A2
inline auto note_frequency(int key) -> double
{
    constexpr double a2_frequency = 110.0;
    constexpr double twelfth_root_two = 1.05946309435929526; // BIG
    return a2_frequency * std::pow(twelfth_root_two, key);
}

}
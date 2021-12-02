// Some convenience from C++23 that I want now
#pragma once
#include <utility>
#include <type_traits>
#include <ranges>

namespace blga {

inline constexpr auto enumerate = std::views::transform([start = 0](auto&& val) mutable {
    return std::make_pair(start++, std::forward<decltype(val)>(val));
});

template <class Enum>
constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}

}
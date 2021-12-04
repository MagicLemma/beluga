// Some convenience from C++23 that I want now
#pragma once
#include <cmath>
#include <utility>
#include <type_traits>
#include <ranges>

namespace blga {

enum class key_name
{
	C  = 0,
	Cs = 1, Db = Cs,
	D  = 2,
	Ds = 3, Eb = Ds,
	E  = 4,
	F  = 5,
	Fs = 6, Gb = Fs,
	G  = 7,
	Gs = 8, Ab = Gs,
	A  = 9, // A2
	As = 10, Bb = As,
	B  = 11,
};

inline constexpr auto enumerate = std::views::transform([start = 0](auto&& val) mutable {
    return std::make_pair(start++, std::forward<decltype(val)>(val));
});

template <class Enum>
constexpr auto to_underlying(Enum e) noexcept -> std::underlying_type_t<Enum>
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}

inline auto note_frequency(const int octave, const key_name k) -> double
{
	constexpr double a2_frequency = 110.0;
	constexpr double twelfth_root_two = 1.05946309435929526456182529494634170077920; // BIG
	
	// The -2 and -9 offsets come from the fact that we are tuned to A2 (octave 2, key 9).
	const auto key = 12 * (octave - 2) + (blga::to_underlying(k) - 9);
	return a2_frequency * std::pow(twelfth_root_two, key);
}

}
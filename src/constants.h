#pragma once
#pragma comment(lib, "winmm.lib")
#define NOMINMAX
#include <Windows.h>

#include <array>
#include <string_view>

namespace blga {

constexpr auto sample_rate = 44100u;
constexpr auto num_blocks = std::size_t{8};
constexpr auto samples_per_block = std::size_t{512};
constexpr auto format = WAVEFORMATEX{
	.wFormatTag = WAVE_FORMAT_PCM,
	.nChannels = 1,
	.nSamplesPerSec = sample_rate,
	.nAvgBytesPerSec = sample_rate * sizeof(short),
	.nBlockAlign = sizeof(short),
	.wBitsPerSample = sizeof(short) * 8,
	.cbSize = 0
};

constexpr auto keyboard = std::array{
	'Z', 'S', 'X', 'D', 'C', 'V', 'G', 'B', 'H', 'N', 'J', 'M',
	'\xbc', 'L', '\xbe', '\xbd', '\xbf'
};

constexpr auto keyboard_ascii = std::string_view{
	"|   |   | |   |   |   |   | |   | |   |   |   |   | |   |   |\n"
	"|   | S | | D |   |   | G | | H | | J |   |   | L | | ; |   |\n"
	"|   |___| |___|   |   |___| |___| |___|   |   |___| |___|   |\n"
	"|     |     |     |     |     |     |     |     |     |     |\n"
	"|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |\n"
	"|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|\n"
};

}
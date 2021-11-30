#include <cmath>
#include <iostream>
#include <algorithm>
#include <numbers>

#include <fmt/format.h>

#include "noise_maker.h"
#include "helpers.h"

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

constexpr double note_frequency(const int octave, const key_name k)
{
	constexpr double A2_FREQUENCY = 110.0;
	
	// The -2 and -9 offsets come from the fact that we are tuned to A2 (octave 2, key 9).
	const auto key = 12 * (octave - 2) + (blga::to_underlying(k) - 9);
	return A2_FREQUENCY * blga::power_of_12th_root_two(key);
}

constexpr std::array keyboard = {
	'Z', 'S', 'X', 'D', 'C', 'V', 'G', 'B', 'H', 'N', 'J', 'M',
	'\xbc', 'L', '\xbe', '\xbd', '\xbf'
};

int main()
{
	std::atomic<double> frequency = 0.0;

	// Display a keyboard
	std::cout << std::endl <<
		"|   |   | |   |   |   |   | |   | |   |   |   |   | |   |   |\n" <<
		"|   | S | | D |   |   | G | | H | | J |   |   | L | | ; |   |\n" <<
		"|   |___| |___|   |   |___| |___| |___|   |   |___| |___|   |\n" <<
		"|     |     |     |     |     |     |     |     |     |     |\n" <<
		"|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |\n" <<
		"|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|\n" << std::endl;

	// Create sound machine!!
	auto sound = blga::noise_maker{};

	// Link noise function with sound machine
	sound.set_noise_function([&](double dt) {
		return 0.2 * std::sin(2.0 * std::numbers::pi * frequency * dt);
	});


	char curr_key = '\0';
	while (true)
	{ 
		bool key_pressed = false;
		for (auto [index, key] : keyboard | blga::enumerate()) {

			if (GetAsyncKeyState((unsigned char)key) & 0x8000) {
				if (curr_key != key) {	
					frequency = note_frequency(3, key_name{index});
					std::cout << "\rNote On : " << frequency << "Hz";
					curr_key = key;
				}

				key_pressed = true;
			}
		}

		if (GetAsyncKeyState('A') & 0x8000) {
			break;
		}

		if (!key_pressed) {	
			if (curr_key != '\0') {
				std::cout << "\rNote Off                        ";
				curr_key = '\0';
			}

			frequency = 0.0;
		}
	}

	sound.stop();
	return 0;
}

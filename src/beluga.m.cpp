// License: https://github.com/OneLoneCoder/videos/blob/master/LICENSE
#include <fmt/format.h>

#include <cmath>
#include <iostream>
#include <algorithm>
#include <numbers>

#include "olcNoiseMaker.h"
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

int main()
{
	std::atomic<double> frequency = 0.0;

	std::vector<std::string> devices = get_devices();

	// Display findings
	for (auto d : devices) {
		std::cout << "Found Output Device: " << d << std::endl;
	}

	std::cout << "Using Device: " << devices[0] << std::endl;

	// Display a keyboard
	std::cout << std::endl <<
		"|   |   | |   |   |   |   | |   | |   |   |   |   | |   |   |\n" <<
		"|   | S | | D |   |   | G | | H | | J |   |   | L | | ; |   |\n" <<
		"|   |___| |___|   |   |___| |___| |___|   |   |___| |___|   |\n" <<
		"|     |     |     |     |     |     |     |     |     |     |\n" <<
		"|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |\n" <<
		"|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|\n" << std::endl;

	// Create sound machine!!
	noise_maker sound(devices[0], 44100, 1, 8, 512);

	// Link noise function with sound machine
	sound.SetUserFunction([&](double dt) {
		return 0.2 * std::sin(2.0 * std::numbers::pi * frequency * dt);
	});

	int current_key = -1;	
	bool key_pressed = false;
	while (1)
	{ 
		key_pressed = false;
		for (int k = 0; k < 16; k++)
		{
			if (GetAsyncKeyState((unsigned char)("ZSXDCVGBHNJM\xbcL\xbe\xbd\xbf"[k])) & 0x8000)
			{
				if (current_key != k)
				{	
					frequency = note_frequency(3, key_name{k});
					std::cout << "\rNote On : " << sound.GetTime() << "s " << frequency << "Hz";					
					current_key = k;
				}

				key_pressed = true;
			}
		}

		if (!key_pressed)
		{	
			if (current_key != -1)
			{
				std::cout << "\rNote Off: " << sound.GetTime() << "s                        ";
				current_key = -1;
			}

			frequency = 0.0;
		}
	}

	return 0;
}

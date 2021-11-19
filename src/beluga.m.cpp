/*
	OneLoneCoder.com - Simple Audio Noisy Thing
	"Allows you to simply listen to that waveform!" - @Javidx9

	License
	~~~~~~~
	Copyright (C) 2018  Javidx9
	This program comes with ABSOLUTELY NO WARRANTY.
	This is free software, and you are welcome to redistribute it
	under certain conditions; See license for details. 
	Original works located at:
	https://www.github.com/onelonecoder
	https://www.onelonecoder.com
	https://www.youtube.com/javidx9

	GNU GPLv3
	https://github.com/OneLoneCoder/videos/blob/master/LICENSE

	From Javidx9 :)
	~~~~~~~~~~~~~~~
	Hello! Ultimately I don't care what you use this for. It's intended to be 
	educational, and perhaps to the oddly minded - a little bit of fun. 
	Please hack this, change it and use it in any way you see fit. You acknowledge 
	that I am not responsible for anything bad that happens as a result of 
	your actions. However this code is protected by GNU GPLv3, see the license in the
	github repo. This means you must attribute me if you use it. You can view this
	license here: https://github.com/OneLoneCoder/videos/blob/master/LICENSE
	Cheers!

	Author
	~~~~~~

	Twitter: @javidx9
	Blog: www.onelonecoder.com

	Versions
	~~~~~~~~

	This is the first version of the software. It presents a simple keyboard and a sine
	wave oscillator.

	See video: https://youtu.be/tgamhuQnOkM

*/
#include <fmt/format.h>

#include <cmath>
#include <iostream>
#include <algorithm>

#include "olcNoiseMaker.h"
#include "helpers.h"

constexpr double TWELFTH_ROOT_TWO = 1.05946309435929526456182529494634170077920;

enum class key
{
	A  = 0,
	As = 1, Bb = As,
	B  = 2,
	C  = 3,
	Cs = 4, Db = Cs,
	D  = 5,
	Ds = 6, Eb = Ds,
	E  = 7,
	F  = 8,
	Fs = 9, Gb = Fs,
	G  = 10
};

enum class octave {};

constexpr double note(const octave o, const key k)
{
	constexpr double A2 = 110.0;
	return A2 * blga::power(TWELFTH_ROOT_TWO, blga::to_underlying(k));
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
		"|   |   |   |   |   | |   |   |   |   | |   | |   |   |   |  \n" <<
		"|   | S |   |   | F | | G |   |   | J | | K | | L |   |   |  \n" <<
		"|   |___|   |   |___| |___|   |   |___| |___| |___|   |   |__\n" <<
		"|     |     |     |     |     |     |     |     |     |     |\n" <<
		"|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |\n" <<
		"|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|\n" << std::endl;

	// Create sound machine!!
	noise_maker sound(devices[0], 44100, 1, 8, 512);

	// Link noise function with sound machine
	sound.SetUserFunction([&](double dt) {
		return 0.5 * std::sin(frequency * 2.0 * 3.14159 * dt);
	});

	// Sit in loop, capturing keyboard state changes and modify
	// synthesizer output accordingly
	int nCurrentKey = -1;	
	bool bKeyPressed = false;
	while (1)
	{
		bKeyPressed = false;
		for (int k = 0; k < 16; k++)
		{
			if (GetAsyncKeyState((unsigned char)("ZSXCFVGBNJMK\xbcL\xbe\xbf"[k])) & 0x8000)
			{
				if (nCurrentKey != k)
				{					
					frequency = note(octave{2}, key{k});
					std::cout << "\rNote On : " << sound.GetTime() << "s " << frequency << "Hz";					
					nCurrentKey = k;
				}

				bKeyPressed = true;
			}
		}

		if (!bKeyPressed)
		{	
			if (nCurrentKey != -1)
			{
				std::cout << "\rNote Off: " << sound.GetTime() << "s                        ";
				nCurrentKey = -1;
			}

			frequency = 0.0;
		}
	}

	return 0;
}

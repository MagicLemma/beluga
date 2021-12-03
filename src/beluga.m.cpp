#include "constants.h"
#include "helpers.h"
#include "noise_maker.h"
#include "envelope.h"
#include "instrument.h"

#include <fmt/format.h>

#include <cmath>
#include <algorithm>
#include <numbers>
#include <optional>

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

double note_frequency(const int octave, const key_name k)
{
	constexpr double a2_frequency = 110.0;
	constexpr double twelfth_root_two = 1.05946309435929526456182529494634170077920; // BIG
	
	// The -2 and -9 offsets come from the fact that we are tuned to A2 (octave 2, key 9).
	const auto key = 12 * (octave - 2) + (blga::to_underlying(k) - 9);
	return a2_frequency * std::pow(twelfth_root_two, key);
}

bool is_key_down(char key)
{
	return GetAsyncKeyState(static_cast<unsigned char>(key)) & 0x8000;
}

int main()
{
	fmt::print(blga::keyboard_ascii);

    auto kb = blga::instrument{
        0.0,
        blga::envelope{
            .attack_time = 0.01,
            .decay_time = 0.01,
            .release_time = 1.0,
            .start_amplitude = 1.2,
            .sustain_amplitude = 0.8
        },
        [&](double frequency, double dt) {
            constexpr auto two_pi = 2.0 * std::numbers::pi;
            const auto lfo = 0.003 * frequency * std::sin(two_pi * 5.0 * dt);

            return std::sin(two_pi * frequency * dt + lfo);
        }
    };

	auto sound = blga::noise_maker{kb};

	std::optional<char> curr_key = {};

	while (!is_key_down('A')) { 
		bool key_pressed = false;

		for (auto [index, key] : blga::enumerate(blga::keyboard)) {
			if (is_key_down(key)) {
				if (curr_key != key) {
                    auto freq = note_frequency(3, key_name{index});
					fmt::print("\rNote On: {} Hz", freq);
                    sound.get_instrument().note_on(
                        sound.get_time(), freq
                    );
					curr_key = key;
				}

				key_pressed = true;
			}
		}

		if (!key_pressed) {	
			if (curr_key.has_value()) {
				fmt::print("\rNote Off                        ");
                sound.get_instrument().note_off(sound.get_time());
				curr_key = std::nullopt;
			}
		}
	}

	return 0;
}

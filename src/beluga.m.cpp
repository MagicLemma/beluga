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

auto note_frequency(const int octave, const key_name k) -> double
{
	constexpr double a2_frequency = 110.0;
	constexpr double twelfth_root_two = 1.05946309435929526456182529494634170077920; // BIG
	
	// The -2 and -9 offsets come from the fact that we are tuned to A2 (octave 2, key 9).
	const auto key = 12 * (octave - 2) + (blga::to_underlying(k) - 9);
	return a2_frequency * std::pow(twelfth_root_two, key);
}

auto is_key_down(char key) -> bool
{
	return GetAsyncKeyState(static_cast<unsigned char>(key)) & 0x8000;
}

auto main() -> int
{
	fmt::print(blga::keyboard_ascii);

    auto kb = blga::instrument{
        0.0,
        blga::envelope{
            .attack_time = 0.01,
            .decay_time = 0.01,
            .release_time = 0.3,
            .start_amplitude = 1.2,
            .sustain_amplitude = 0.8
        },
        [&](double frequency, double dt) {
            constexpr auto two_pi = 2.0 * std::numbers::pi;
            const auto lfo = 0.0 * frequency * std::sin(two_pi * 5.0 * dt);
			
			double amp = 0.0;
			for (double i = 1; i < 10; ++i) {
				amp += std::sin(two_pi * frequency * i * dt + lfo) / i;
			}
			return amp / 10;
        }
    };

	auto sound = blga::noise_maker{kb};

	while (!is_key_down('A')) {
		auto& instrument = sound.get_instrument();
		for (auto [index, key] : blga::enumerate(blga::keyboard)) {
			auto key_down = is_key_down(key);
			auto active = instrument.is_note_active(key);
			auto freq = note_frequency(3, key_name{index});

			if (key_down && !active) {
				sound.get_instrument().note_on(key, sound.get_time(), freq);
			}
			else if (!key_down && active) {
				sound.get_instrument().note_off(key, sound.get_time());
			}
		}
	}

	return 0;
}

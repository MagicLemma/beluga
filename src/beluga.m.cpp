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

auto is_key_down(char key) -> bool
{
	return GetAsyncKeyState(static_cast<unsigned char>(key)) & 0x8000;
}

auto main() -> int
{
	fmt::print(blga::keyboard_ascii);

    auto kb = blga::instrument{
        blga::envelope{
            .attack_time = 0.01,
            .decay_time = 0.01,
            .release_time = 0.3,
            .start_amplitude = 1.2,
            .sustain_amplitude = 0.8
        },
        [](double frequency, double time) {
            constexpr auto two_pi = 2.0 * std::numbers::pi;
            const auto lfo = 0.0 * frequency * std::sin(two_pi * 5.0 * time);

			double amp = 0.0;
			for (double i = 1; i < 10; ++i) {
				amp += std::sin(two_pi * frequency * i * time + lfo) / i;
			}
			return amp / 10;
        }
    };

	auto sound = blga::noise_maker{kb};

	while (!is_key_down('A')) {
		auto lock = std::unique_lock{sound.get_instrument_mtx()};
		auto& instrument = sound.get_instrument();
		
		for (auto [index, key] : blga::enumerate(blga::keyboard)) {
			auto key_down = is_key_down(key);
			auto active = instrument.is_note_active(key);
			auto time = sound.get_time();

			if (key_down && !active) {
				fmt::print("\rNote {} Hz", blga::note_frequency(15 + index));
				instrument.note_on(key, time);
			}
			else if (!key_down && active) {
				fmt::print("\rNote off                         ");
				instrument.note_off(key, time);
			}
		}
	}

	return 0;
}

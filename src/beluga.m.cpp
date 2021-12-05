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

	std::unordered_map<char, bool> input;
	const auto is_key_active = [&](char k) {
		if (auto it = input.find(k); it != input.end()) {
			return it->second;
		}
		return false;
	};

	while (!is_key_down('A')) {
		auto lock = std::unique_lock{sound.get_instrument_mtx()};
		auto& instrument = sound.get_instrument();
		
		for (auto [index, key] : blga::enumerate(blga::keyboard)) {
			int k = index + 15;
			auto key_down = is_key_down(key);
			auto active = is_key_active(key);
			auto time = sound.get_time();

			if (key_down && !active) {
				instrument.note_on(k, time);
				input[key] = true;
			}
			else if (!key_down && active) {
				instrument.note_off(k, time);
				input[key] = false;
			}
		}
	}

	return 0;
}

#include "constants.h"
#include "helpers.h"
#include "noise_maker.h"
#include "components.h"

#include <sprocket/core/game_loop.h>
#include <sprocket/core/window.h>
#include <sprocket/core/events.h>
#include <sprocket/core/input.h>

#include <fmt/format.h>

#include <cmath>
#include <algorithm>
#include <numbers>
#include <optional>

const auto instrument = blga::instrument{
    .envelope = blga::envelope{
        .attack_time = 0.01,
        .decay_time = 0.01,
        .release_time = 0.3,
        .start_amplitude = 1.2,
        .sustain_amplitude = 0.8
    },
    .oscillator = [](double frequency, double time) {
        constexpr auto two_pi = 2.0 * std::numbers::pi;
        const auto lfo = 0.0 * frequency * std::sin(two_pi * 5.0 * time);

        double amp = 0.0;
        for (double i = 1; i < 10; ++i) {
            amp += std::sin(two_pi * frequency * i * time + lfo) / i;
        }
        return amp / 10;
    }
};

constexpr auto keyboard = std::array{
    spkt::Keyboard::Z,
    spkt::Keyboard::S,
    spkt::Keyboard::X,
    spkt::Keyboard::D,
    spkt::Keyboard::C,
    spkt::Keyboard::V,
    spkt::Keyboard::G,
    spkt::Keyboard::B,
    spkt::Keyboard::H,
    spkt::Keyboard::N,
    spkt::Keyboard::J,
    spkt::Keyboard::M,
    spkt::Keyboard::COMMA,
    spkt::Keyboard::L,
    spkt::Keyboard::PERIOD,
    spkt::Keyboard::SEMI_COLON,
    spkt::Keyboard::FORWARD_SLASH
};

class beluga
{
    spkt::window* d_window;

    blga::noise_maker d_sound;

public:
    beluga(spkt::window* window)
        : d_window(window)
        , d_sound{}
    {
        d_sound.add_channel(instrument);
    }

    void on_update(double dt) {}

    void on_event(spkt::event& event)
    {
        if (auto data = event.get_if<spkt::keyboard_pressed_event>()) {
            if (data->key == spkt::Keyboard::A) {
                d_window->close();
                return;
            }
            // key == musical keyboard, button = computer keyboard
            for (auto [key, button] : keyboard | blga::enumerate(15)) {
                if (data->key == button) {
                    d_sound.note_on(key, 0);
                }
            }
        }
        else if (auto data = event.get_if<spkt::keyboard_released_event>()) {
            for (auto [key, button] : keyboard | blga::enumerate(15)) {
                if (data->key == button) {
                    d_sound.note_off(key, 0);
                }
            }
        }
    }
};

auto main() -> int
{
    return spkt::run_app<beluga>("beluga");
}

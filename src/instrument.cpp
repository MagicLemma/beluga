#include "instrument.h"
#include "constants.h"
#include "helpers.h"

namespace blga {

instrument::instrument(
    const blga::envelope& envelope,
    const blga::oscillator& oscillator
)
    : d_envelope(envelope)
    , d_oscillator(oscillator)
{
    // Create the keyboard
    for (auto [index, key] : blga::enumerate(blga::keyboard)) {
        d_notes[key] = {
            .frequency = blga::note_frequency(3, key_name{index}),
            .toggle_time = -1.0,
            .active = false
        };
    }
}

auto instrument::note_on(char note, double dt) -> void
{
    d_notes[note].toggle_time = dt;
    d_notes[note].active = true;
}

auto instrument::note_off(char note, double dt) -> void
{
    d_notes[note].toggle_time = dt;
    d_notes[note].active = false;
}

auto instrument::amplitude(double dt) -> double
{
    double amp = 0.0;
    for (const auto& [key, note] : d_notes) {
        amp += (
            d_envelope.amplitude(dt, note.toggle_time, note.active) *
            d_oscillator(note.frequency, dt)
        );
    }
    return amp;
}

auto instrument::is_note_active(char note) const -> bool
{
    if (auto it = d_notes.find(note); it != d_notes.end()) {
        return it->second.active;
    }
    return false;
}

}
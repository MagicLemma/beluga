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
    for (auto [index, keyboard_button] : blga::enumerate(blga::keyboard)) {
        d_notes[15 + index] = {
            .key = 15 + index, // Bumped up by 15 so that the first note is C3
            .toggle_time = -1.0,
            .active = false
        };
    }
}

auto instrument::note_on(int note, double time) -> void
{
    d_notes[note].toggle_time = time;
    d_notes[note].active = true;
}

auto instrument::note_off(int note, double time) -> void
{
    d_notes[note].toggle_time = time;
    d_notes[note].active = false;
}

auto instrument::amplitude(double time) -> double
{
    double amp = 0.0;
    for (const auto& [key, note] : d_notes) {
        if (note.active || time < note.toggle_time + d_envelope.release_time) {
            amp += d_envelope.amplitude(time, note.toggle_time, note.active) *
                d_oscillator(blga::note_frequency(note.key), time);
        }
    }
    return amp;
}

auto instrument::is_note_active(int note) const -> bool
{
    if (auto it = d_notes.find(note); it != d_notes.end()) {
        return it->second.active;
    }
    return false;
}

}
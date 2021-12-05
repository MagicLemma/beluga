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
    auto count = std::erase_if(d_notes, [&](const auto& it) {
        const auto& [key, note] = it;
        if (note.active || time < note.toggle_time + d_envelope.release_time) {
            amp += d_envelope.amplitude(time, note.toggle_time, note.active) *
                d_oscillator(blga::note_frequency(key), time);
            return false;
        }
        return true; // Delete all notes that are done
    });
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
#include "instrument.h"
#include "constants.h"
#include "helpers.h"

#include <fmt/format.h>

namespace blga {

instrument::instrument(
    const blga::envelope& envelope,
    const blga::oscillator& oscillator
)
    : d_envelope(envelope)
    , d_oscillator(oscillator)
{
}

auto instrument::note_on(int key, double time) -> void
{
    d_notes.emplace_back(key, time, true);
}

auto instrument::note_off(int key, double time) -> void
{
    for (auto& note : d_notes) {
        if (note.key == key && note.active) {
            note.active = false;
            note.toggle_time = time;
        }
    }
}

auto instrument::amplitude(double time) -> double
{
    double amp = 0.0;
    auto count = std::erase_if(d_notes, [&](const auto& note) {
        if (note.active || time < note.toggle_time + d_envelope.release_time) {
            amp += d_envelope.amplitude(time, note.toggle_time, note.active) *
                d_oscillator(blga::note_frequency(note.key), time);
            return false;
        }
        return true; // Delete all notes that are done
    });
    return amp;
}

}
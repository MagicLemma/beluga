#include "instrument.h"

namespace blga {

instrument::instrument(
    double frequency,
    const blga::envelope& envelope,
    const blga::oscillator& oscillator
)
    : d_frequency(frequency)
    , d_envelope(envelope)
    , d_oscillator(oscillator)
{
}

auto instrument::note_on(char note, double dt, double frequency) -> void
{
    d_notes[note].on = dt;
    d_notes[note].active = true;
    d_notes[note].frequency = frequency;
}

auto instrument::note_off(char note, double dt) -> void
{
    d_notes[note].off = dt;
    d_notes[note].active = false;
}

auto instrument::amplitude(double dt) -> double
{
    double amp = 0.0;
    for (const auto& [key, note] : d_notes) {
        amp += (
            d_envelope.amplitude(dt, note.on, note.off, note.active) *
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
#include "components.h"
#include "helpers.h"

namespace blga {

auto amplitude(
    const blga::note& note,
    const blga::instrument& instrument,
    double time) -> double
{
    const auto note_time = time - note.toggle_time;
    const auto& [envelope, oscillator] = instrument;

    double amp = 0.0;
    if (note.active) {
        if (note_time < envelope.attack_time) { // Attack
            amp = (note_time / envelope.attack_time) * envelope.start_amplitude;
        }
        else if (note_time < envelope.attack_time + envelope.decay_time) { // Decay
            amp = ((note_time - envelope.attack_time) / envelope.decay_time)  *
                    (envelope.sustain_amplitude - envelope.start_amplitude) +
                    envelope.start_amplitude;
        }
        else {
            amp = envelope.sustain_amplitude; // Sustain
        }
    }
    else {
        amp = (1.0 - (note_time / envelope.release_time)) * envelope.sustain_amplitude;
    }

    return std::max(amp, 0.0) * oscillator(blga::note_frequency(note.key), time);
}

}
#pragma once
#include <cmath>

namespace blga {

struct envelope
{
    double attack_time;
    double decay_time;
    double release_time;

    double start_amplitude;
    double sustain_amplitude;

    auto amplitude(double dt, double on, double off, bool active) const -> double
    {
        double amp = 0.0;

        if (active) {
            auto note_time = dt - on;

            if (note_time < attack_time) { // Attack
                amp = (note_time / attack_time) * start_amplitude;
            }
            else if (note_time < attack_time + decay_time) { // Decay
                amp = ((note_time - attack_time) / decay_time)  *
                      (sustain_amplitude - start_amplitude) +
                      start_amplitude;
            }
            else {
                amp = sustain_amplitude; // Sustain
            }
        }
        else {
            auto note_time = dt - off;
            amp = (1.0 - (note_time / release_time)) * sustain_amplitude;
        }

        return std::max(amp, 0.0);
    }
};

}
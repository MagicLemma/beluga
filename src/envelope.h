#pragma once

namespace blga {

auto cutoff(double amplitude) -> double
{
    return amplitude > 0.001 ? amplitude : 0.0;
}

struct envelope
{
    double attack_time;
    double decay_time;
    double release_time;

    double start_amplitude;
    double sustain_amplitude;

    double on_time = 0.0;
    double off_time = 0.0;

    bool note_on = false;

    auto amplitude(double dt) const -> double
    {
        if (note_on) {
            auto note_time = dt - on_time;

            if (note_time < attack_time) { // Attack
                return cutoff((note_time / attack_time) * start_amplitude);
            }

            if (note_time < attack_time + decay_time) { // Decay
                return cutoff(
                    ((note_time - attack_time) / decay_time)  *
                    (sustain_amplitude - start_amplitude) +
                    start_amplitude
                );
            }

            return cutoff(sustain_amplitude); // Sustain

        }

        auto note_time = dt - off_time;
        return cutoff((1.0 - (note_time / release_time)) * sustain_amplitude);
    }
};

}
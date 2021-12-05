#pragma once
#include <cmath>
#include <functional>

namespace blga {

using oscillator = std::function<double(double, double)>;

struct envelope
{
    double attack_time;
    double decay_time;
    double release_time;

    double start_amplitude;
    double sustain_amplitude;
};

struct note
{
    int    key;
    double toggle_time; // Time it was last toggle on or off
    bool   active;
};

struct instrument
{
    blga::envelope   envelope;
    blga::oscillator oscillator;
};

double amplitude(
    const blga::note& note,
    const blga::instrument& instrument,
    double time
);

}
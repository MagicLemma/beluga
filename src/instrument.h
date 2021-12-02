#pragma once
#include "envelope.h"

#include <atomic>
#include <functional>

namespace blga {

using oscillator = std::function<double(double, double)>;

class instrument
{
    std::atomic<double> d_frequency;
    blga::envelope      d_envelope;
    blga::oscillator    d_oscillator;

public:
    instrument(
        double frequency,
        const blga::envelope& envelope,
        const blga::oscillator& oscillator
    );

    void note_on(double dt);
    void note_off(double dt);

    auto amplitude(double dt) -> double;

    void set_frequency(double new_frequency) { d_frequency = new_frequency; }
};

}
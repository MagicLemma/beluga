#pragma once
#include "envelope.h"

#include <atomic>
#include <functional>
#include <vector>

namespace blga {

using oscillator = std::function<double(double, double)>;

struct note
{
    int    key;
    double toggle_time; // Time it was last toggle on or off
    bool   active;
};

struct instrument
{
    blga::envelope   d_envelope;
    blga::oscillator d_oscillator;
};


}
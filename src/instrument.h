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

class instrument
{
    std::vector<note> d_notes; // Temporary, generalise later
    blga::envelope    d_envelope;
    blga::oscillator  d_oscillator;

public:
    instrument(const blga::envelope& envelope, const blga::oscillator& oscillator);

    auto note_on(int note, double time) -> void;
    auto note_off(int note, double time) -> void;

    auto amplitude(double time) -> double;
};


}
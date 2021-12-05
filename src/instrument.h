#pragma once
#include "envelope.h"

#include <atomic>
#include <functional>

namespace blga {

using oscillator = std::function<double(double, double)>;

struct note
{
    double toggle_time; // Time it was last toggle on or off
    bool   active;
};

class instrument
{
    std::unordered_map<int, note> d_notes; // Temporary, generalise later
    blga::envelope   d_envelope;
    blga::oscillator d_oscillator;

public:
    instrument(const blga::envelope& envelope, const blga::oscillator& oscillator);

    auto note_on(int note, double time) -> void;
    auto note_off(int note, double time) -> void;
    auto is_note_active(int note) const -> bool;

    auto amplitude(double time) -> double;
};


}
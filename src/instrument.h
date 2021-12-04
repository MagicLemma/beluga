#pragma once
#include "envelope.h"

#include <atomic>
#include <functional>

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
    std::unordered_map<char, note> d_notes; // Temporary, generalise later
    blga::envelope   d_envelope;
    blga::oscillator d_oscillator;

public:
    instrument(const blga::envelope& envelope, const blga::oscillator& oscillator);

    auto note_on(char note, double time) -> void;
    auto note_off(char note, double time) -> void;
    auto is_note_active(char note) const -> bool;

    auto amplitude(double time) -> double;
};


}
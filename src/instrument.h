#pragma once
#include "envelope.h"

#include <atomic>
#include <functional>

namespace blga {

using oscillator = std::function<double(double, double)>;

struct note
{
    double frequency;
    double on;
    double off;
    bool active;
};

class instrument
{
    std::unordered_map<char, note> d_notes; // Temporary, generalise later
    double           d_frequency;
    blga::envelope   d_envelope;
    blga::oscillator d_oscillator;

public:
    instrument(
        double frequency,
        const blga::envelope& envelope,
        const blga::oscillator& oscillator
    );

    auto note_on(char note, double dt, double frequency) -> void;
    auto note_off(char note, double dt) -> void;
    auto is_note_active(char note) const -> bool;

    auto amplitude(double dt) -> double;
};


}
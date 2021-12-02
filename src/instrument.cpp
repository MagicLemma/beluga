#include "instrument.h"

namespace blga {

instrument::instrument(
    double frequency,
    const blga::envelope& envelope,
    const blga::oscillator& oscillator
)
    : d_frequency(frequency)
    , d_envelope(envelope)
    , d_oscillator(oscillator)
{
}

void instrument::note_on(double dt)
{
    d_envelope.on_time = dt;
    d_envelope.note_on = true;
}

void instrument::note_off(double dt)
{
    d_envelope.off_time = dt;
    d_envelope.note_on = false;
}

auto instrument::amplitude(double dt) -> double
{
    return d_envelope.amplitude(dt) * d_oscillator(d_frequency, dt);
}

}
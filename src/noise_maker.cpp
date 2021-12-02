#include "noise_maker.h"

#include "constants.h"

#include <Windows.h>

#include <cmath>
#include <vector>
#include <string>
#include <limits>

namespace blga {
namespace {

auto scale(double value) -> short
{
	return static_cast<short>(std::clamp(value, -1.0, 1.0) * std::numeric_limits<short>::max());
}

}

noise_maker::noise_maker(const blga::instrument& instrument)
    : d_instrument{instrument}
    , d_audio_buffer{}
    , d_thread{}
    , d_ready{true}
    , d_time{0.0}
    , d_semaphore{num_blocks}
{
    // Callback passed to WaveOpen. Releases the semaphore to request more data.
    const auto cb = +[](HWAVEOUT, UINT msg, DWORD_PTR user_data, DWORD_PTR, DWORD_PTR) {
        if (msg == WOM_DONE) {
            auto semaphore = (std::counting_semaphore<num_blocks>*)user_data;
            semaphore->release();
        }
    };

    auto device = HWAVEOUT{};
    const auto rc = waveOutOpen(
        &device, 0, &format, (DWORD_PTR)cb, (DWORD_PTR)&d_semaphore, CALLBACK_FUNCTION
    );

    if (rc != S_OK) {
        throw std::exception("Could not open sound device");
    }

    // Thread that acquires the semaphore to send more data to WaveOpen.
    d_thread = std::thread([&, device]{
        while (d_ready) {
            d_semaphore.acquire();

            auto& block = d_audio_buffer.next_block();
            for (auto& datum : block.data) {
                datum = scale(d_instrument.amplitude(d_time));
                d_time += 1.0 / sample_rate;
            }

            block.send_to_sound_device(device);
        }
    });
}

void noise_maker::stop()
{
    d_ready = false;
    d_thread.join();
}

}
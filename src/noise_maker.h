#pragma once
#include "constants.h"
#include "sound_buffer.h"

#include <Windows.h>

#include <cmath>
#include <vector>
#include <functional>
#include <string>
#include <thread>
#include <atomic>
#include <limits>
#include <semaphore>

namespace blga {

inline short scale(double value)
{
	return static_cast<short>(std::clamp(value, -1.0, 1.0) * std::numeric_limits<short>::max());
}

class noise_maker
{
	std::function<double(double)> d_callback;

	blga::audio_buffer<num_blocks, samples_per_block> d_audio_buffer;

	std::thread d_thread;
	std::atomic<bool> d_ready;

	std::counting_semaphore<num_blocks> d_semaphore;

public:
	noise_maker()
		: d_callback{}
		, d_audio_buffer{}
		, d_thread{}
		, d_ready{true}
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
			double time = 0.0;
			while (d_ready) {
				d_semaphore.acquire();

				auto& block = d_audio_buffer.next_block();
				for (auto& datum : block.data) {
					datum = scale(d_callback(time));
					time += 1.0 / sample_rate;
				}

				block.send_to_sound_device(device);
			}
		});
	}

	void stop()
	{
		d_ready = false;
		d_thread.join();
	}

	void set_noise_function(const std::function<double(double)>& callback)
	{
		d_callback = callback;
	}
};

}
#pragma once
#pragma comment(lib, "winmm.lib")
#define NOMINMAX
#include <Windows.h>

#include "sound_buffer.h"

#include <cmath>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <limits>
#include <semaphore>

namespace blga {

static constexpr auto short_max = std::numeric_limits<short>::max();
static constexpr auto sample_rate = 44100u;
static constexpr auto num_blocks = std::size_t{8};
static constexpr auto samples_per_block = std::size_t{512};
static constexpr auto wave_format = WAVEFORMATEX{
	.wFormatTag = WAVE_FORMAT_PCM,
	.nChannels = 1,
	.nSamplesPerSec = sample_rate,
	.nAvgBytesPerSec = sample_rate * sizeof(short),
	.nBlockAlign = sizeof(short),
	.wBitsPerSample = sizeof(short) * 8,
	.cbSize = 0
};

class noise_maker
{
	std::function<double(double)> d_callback;

	blga::audio_buffer<num_blocks, samples_per_block> d_audio_buffer;

	HWAVEOUT d_device;

	std::thread d_thread;
	std::atomic<bool> d_ready;

	std::counting_semaphore<num_blocks> d_semaphore;

public:

	noise_maker()
		: d_callback{}
		, d_audio_buffer{}
		, d_device{nullptr}
		, d_thread{}
		, d_ready{true}
		, d_semaphore{num_blocks}
	{
		const auto rc = waveOutOpen(
			&d_device, 0, &wave_format,
			(DWORD_PTR)wave_out_proc_wrap, (DWORD_PTR)&d_semaphore, CALLBACK_FUNCTION
		);

		if (rc != S_OK)
		{
			throw std::exception("Could not open sound device");
		}

		d_thread = std::thread(&noise_maker::main_thread, this);
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


private:

	static void wave_out_proc_wrap(
		HWAVEOUT, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR, DWORD_PTR)
	{
		if (uMsg == WOM_DONE) {
			auto& semaphore = *reinterpret_cast<std::counting_semaphore<num_blocks>*>(dwInstance);
			semaphore.release();
		}
	}

	void main_thread()
	{
		const double dt = 1.0 / sample_rate;

		double time = 0.0;
		while (d_ready)
		{
			d_semaphore.acquire();

			auto block = d_audio_buffer.next_block();
			for (auto& datum : block.data)
			{
				datum = (short)(std::clamp(d_callback(time), -1.0, 1.0) * short_max);
				time += dt;
			}

			block.send_to_sound_device(d_device);
		}
	}
};

}
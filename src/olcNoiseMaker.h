// License: https://github.com/OneLoneCoder/videos/blob/master/LICENSE
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

static constexpr auto short_max = std::numeric_limits<short>::max();
static constexpr auto sample_rate = 44100u;

static constexpr auto wave_format = WAVEFORMATEX{
	.wFormatTag = WAVE_FORMAT_PCM,
	.nChannels = 1,
	.nSamplesPerSec = sample_rate,
	.nAvgBytesPerSec = sample_rate * sizeof(short),
	.nBlockAlign = sizeof(short),
	.wBitsPerSample = sizeof(short) * 8,
	.cbSize = 0
};

std::vector<std::string> get_devices()
{
	int nDeviceCount = waveOutGetNumDevs();
	std::vector<std::string> sDevices;
	WAVEOUTCAPS woc;
	for (int n = 0; n < nDeviceCount; n++)
		if (waveOutGetDevCaps(n, &woc, sizeof(WAVEOUTCAPS)) == S_OK)
			sDevices.push_back(woc.szPname);
	return sDevices;
}

class noise_maker
{
	std::function<double(double)> d_callback;

	blga::audio_buffer d_audio_buffer;

	HWAVEOUT d_device;

	std::thread d_thread;
	std::atomic<bool> d_ready;

	std::counting_semaphore<> d_semaphore;

public:

	noise_maker(std::size_t num_blocks = 8, std::size_t samples_per_block = 512)
		: d_callback{}
		, d_audio_buffer{num_blocks, samples_per_block}
		, d_device{nullptr}
		, d_thread{}
		, d_ready{true}
		, d_semaphore{num_blocks}
	{
		// Open Device if valid
		if (waveOutOpen(&d_device, 0, &wave_format, (DWORD_PTR)wave_out_proc_wrap, (DWORD_PTR)&d_semaphore, CALLBACK_FUNCTION) != S_OK)
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

	inline static void wave_out_proc_wrap(HWAVEOUT hWaveOut, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
	{
		if (uMsg == WOM_DONE) {
			reinterpret_cast<std::counting_semaphore<>*>(dwInstance)->release();
		}
	}

	// Main thread. This loop responds to requests from the soundcard to fill 'blocks'
	// with audio data. If no requests are available it goes dormant until the sound
	// card is ready for more data. The block is fille by the "user" in some manner
	// and then issued to the soundcard.
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

// License: https://github.com/OneLoneCoder/videos/blob/master/LICENSE
#pragma once
#pragma comment(lib, "winmm.lib")
#define NOMINMAX
#include <Windows.h>

#include "sound_buffer.h"

#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <limits>
#include <condition_variable>

static constexpr auto short_max = std::numeric_limits<short>::max();
static constexpr auto sample_rate = 44100u;

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
	std::atomic<std::size_t> d_block_free;
	std::condition_variable d_cv_block_not_zero;
	std::mutex d_mux_block_not_zero;

	std::atomic<double> d_time;

public:

	noise_maker(std::size_t nBlocks = 8, std::size_t nBlockSamples = 512)
		: d_callback{}
		, d_audio_buffer{nBlocks, nBlockSamples}
		, d_device{nullptr}
		, d_thread{}
		, d_ready{true}
		, d_block_free{nBlocks}
		, d_cv_block_not_zero{}
		, d_mux_block_not_zero{}
		, d_time{0}
	{

		// Validate device
		WAVEFORMATEX waveFormat;
		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nSamplesPerSec = sample_rate;
		waveFormat.wBitsPerSample = sizeof(short) * 8;
		waveFormat.nChannels = 1;
		waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
		waveFormat.cbSize = 0;

		// Open Device if valid
		if (waveOutOpen(&d_device, 0, &waveFormat, (DWORD_PTR)wave_out_proc_wrap, (DWORD_PTR)this, CALLBACK_FUNCTION) != S_OK)
		{
			throw std::exception("Could not open sound device");
		}

		d_thread = std::thread(&noise_maker::main_thread, this);

		// Start the ball rolling
		auto lock = std::unique_lock{d_mux_block_not_zero};
		d_cv_block_not_zero.notify_one();
	}

	void Stop()
	{
		d_ready = false;
		d_thread.join();
	}

	double GetTime()
	{
		return d_time;
	}

	void SetUserFunction(const std::function<double(double)>& callback)
	{
		d_callback = callback;
	}


private:

	// Handler for soundcard request for more data
	void wave_out_proc(HWAVEOUT hWaveOut, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
	{
		if (uMsg == WOM_DONE) {
			d_block_free++;
			auto lock = std::unique_lock{d_mux_block_not_zero};
			d_cv_block_not_zero.notify_one();
		}
	}

	// Static wrapper for sound card handler
	inline static void wave_out_proc_wrap(HWAVEOUT hWaveOut, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
	{
		reinterpret_cast<noise_maker*>(dwInstance)->wave_out_proc(hWaveOut, uMsg, dwParam1, dwParam2);
	}

	// Main thread. This loop responds to requests from the soundcard to fill 'blocks'
	// with audio data. If no requests are available it goes dormant until the sound
	// card is ready for more data. The block is fille by the "user" in some manner
	// and then issued to the soundcard.
	void main_thread()
	{
		d_time = 0.0;
		double dt = 1.0 / sample_rate;

		while (d_ready)
		{
			// Wait for block to become available
			if (d_block_free == 0)
			{
				auto lock = std::unique_lock{d_mux_block_not_zero};
				d_cv_block_not_zero.wait(lock);
			}

			// Block is here, so use it
			d_block_free--;

			auto block = d_audio_buffer.next_block();
			for (auto& datum : block.data)
			{
				datum = (short)(std::clamp(d_callback(d_time), -1.0, 1.0) * short_max);
				d_time += dt;
			}

			block.send_to_sound_device(d_device);
		}
	}
};

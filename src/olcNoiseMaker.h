// License: https://github.com/OneLoneCoder/videos/blob/master/LICENSE
#pragma once
#pragma comment(lib, "winmm.lib")

#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <condition_variable>

#include <Windows.h>

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

using T = short;

class noise_maker;

// Cannot use the user data param to pass this into the callback as the API is 32bit, so the pointer gets chopped :/ :/ :/
static noise_maker* g_instance = nullptr;

class noise_maker
{
	std::function<double(double)> d_callback;

	unsigned int d_sample_rate;
	unsigned int d_channels;
	unsigned int d_block_count;
	unsigned int d_block_samples;
	unsigned int d_block_current;

	std::unique_ptr<short[]> d_block_memory;
	std::unique_ptr<WAVEHDR[]> d_wave_headers;
	HWAVEOUT d_device;

	std::thread d_thread;
	std::atomic<bool> d_ready;
	std::atomic<unsigned int> d_block_free;
	std::condition_variable d_cv_block_not_zero;
	std::mutex d_mux_block_not_zero;

	std::atomic<double> d_time;

public:

	noise_maker(std::string sOutputDevice, unsigned int nSampleRate = 44100, unsigned int nChannels = 1, unsigned int nBlocks = 8, unsigned int nBlockSamples = 512)
		: d_callback{}
		, d_sample_rate{nSampleRate}
		, d_channels{nChannels}
		, d_block_count{nBlocks}
		, d_block_samples{nBlockSamples}
		, d_block_current{0}
		, d_block_memory{std::make_unique<short[]>(d_block_count * d_block_samples)}
		, d_wave_headers{std::make_unique<WAVEHDR[]>(d_block_count)}
		, d_device{nullptr}
		, d_thread{}
		, d_ready{true}
		, d_block_free{d_block_count}
		, d_cv_block_not_zero{}
		, d_mux_block_not_zero{}
		, d_time{0}
	{
		if (g_instance) {
			throw std::exception("Can only have one noise maker instance");
		}

		// Validate device
		WAVEFORMATEX waveFormat;
		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nSamplesPerSec = d_sample_rate;
		waveFormat.wBitsPerSample = sizeof(T) * 8;
		waveFormat.nChannels = d_channels;
		waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
		waveFormat.cbSize = 0;

		// Open Device if valid
		if (waveOutOpen(&d_device, 0, &waveFormat, (DWORD_PTR)wave_out_proc_wrap, (DWORD_PTR)this, CALLBACK_FUNCTION) != S_OK)
		{
			throw std::exception("Could not open sound device");
		}

		// Link headers to block memory
		for (unsigned int n = 0; n < d_block_count; n++)
		{
			d_wave_headers[n].dwBufferLength = d_block_samples * sizeof(T);
			d_wave_headers[n].lpData = (LPSTR)(d_block_memory.get() + (n * d_block_samples));
		}

		d_thread = std::thread(&noise_maker::main_thread, this);

		// Start the ball rolling
		auto lock = std::unique_lock{d_mux_block_not_zero};
		d_cv_block_not_zero.notify_one();

		g_instance = this;
	}

	~noise_maker()
	{
		g_instance = nullptr;
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
	void wave_out_proc(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwParam1, DWORD dwParam2)
	{
		if (uMsg == WOM_DONE) {
			d_block_free++;
			auto lock = std::unique_lock{d_mux_block_not_zero};
			d_cv_block_not_zero.notify_one();
		}
	}

	// Static wrapper for sound card handler
	inline static void wave_out_proc_wrap(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
	{
		g_instance->wave_out_proc(hWaveOut, uMsg, dwParam1, dwParam2);
	}

	// Main thread. This loop responds to requests from the soundcard to fill 'blocks'
	// with audio data. If no requests are available it goes dormant until the sound
	// card is ready for more data. The block is fille by the "user" in some manner
	// and then issued to the soundcard.
	void main_thread()
	{
		d_time = 0.0;
		double dt = 1.0 / (double)d_sample_rate;

		// Goofy hack to get maximum integer for a type at run-time
		const double max_sample = (double)((short)std::pow(2, (sizeof(short) * 8) - 1) - 1);

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

			// Prepare block for processing
			if (d_wave_headers[d_block_current].dwFlags & WHDR_PREPARED)
				waveOutUnprepareHeader(d_device, &d_wave_headers[d_block_current], sizeof(WAVEHDR));

			short new_sample = 0;
			
			for (unsigned int n = 0; n < d_block_samples; n++)
			{
				// User Process
				if (d_callback) {
					new_sample = (short)(std::clamp(d_callback(d_time), -1.0, 1.0) * max_sample);
				}

				d_block_memory[d_block_current * d_block_samples + n] = new_sample;
				d_time += dt;
			}

			// Send block to sound device
			waveOutPrepareHeader(d_device, &d_wave_headers[d_block_current], sizeof(WAVEHDR));
			waveOutWrite(d_device, &d_wave_headers[d_block_current], sizeof(WAVEHDR));
			d_block_current++;
			d_block_current %= d_block_count;
		}
	}
};

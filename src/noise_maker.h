#pragma once
#include "constants.h"
#include "audio_buffer.h"

#include <atomic>
#include <functional>
#include <semaphore>
#include <thread>

namespace blga {

class noise_maker
{
	std::function<double(double)> d_callback;

	blga::audio_buffer<blga::num_blocks, blga::samples_per_block> d_audio_buffer;

	std::thread d_thread;
	std::atomic<bool> d_ready;

	std::counting_semaphore<num_blocks> d_semaphore;

public:
	noise_maker();

	void stop();

	void set_noise_function(const std::function<double(double)>& callback);
};

}
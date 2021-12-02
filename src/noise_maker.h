#pragma once
#include "constants.h"
#include "audio_buffer.h"
#include "instrument.h"

#include <atomic>
#include <functional>
#include <semaphore>
#include <thread>

namespace blga {

class noise_maker
{
    // Fix the race condition in accessing this (mostly will go when we switch to
    // using notes)
    blga::instrument d_instrument;

	blga::audio_buffer<blga::num_blocks, blga::samples_per_block> d_audio_buffer;

	std::thread d_thread;
	std::atomic<bool> d_ready;
    std::atomic<double> d_time;

	std::counting_semaphore<num_blocks> d_semaphore;

public:
	noise_maker(const blga::instrument& instrument);

	void stop();

    double get_time() const { return d_time; }

    blga::instrument& get_instrument() { return d_instrument; }
};

}
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

	std::jthread d_thread;
	std::atomic<bool> d_ready;
    std::atomic<double> d_time;

	std::counting_semaphore<num_blocks> d_semaphore;

public:
	noise_maker(const blga::instrument& instrument);
    ~noise_maker();

    auto get_time() const -> double { return d_time; }
    auto get_instrument() -> blga::instrument& { return d_instrument; }
};

}
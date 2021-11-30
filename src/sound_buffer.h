#pragma once
#include <Windows.h>

#include <vector>
#include <span>

namespace blga {

struct block_proxy
{
    WAVEHDR*         header;
    std::span<short> data;

    void send_to_sound_device(HWAVEOUT device) const;
};

class audio_buffer
{
    struct block
    {
        WAVEHDR            header;
        std::vector<short> data;
    };

    std::vector<block> d_blocks;
    std::size_t        d_current;
  
    audio_buffer(const audio_buffer&) = delete;
    audio_buffer& operator=(const audio_buffer&) = delete;

public:
    audio_buffer(std::size_t num_blocks, std::size_t samples_per_block);

    block_proxy next_block();
};

}
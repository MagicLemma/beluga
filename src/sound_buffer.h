#pragma once
#pragma comment(lib, "winmm.lib")
#include <Windows.h>

#include <vector>
#include <span>

namespace blga {


class audio_buffer
{
    struct block
    {
        std::vector<short> data;
        WAVEHDR            header;
    };

    std::vector<block> d_blocks;
    std::size_t        d_current;

    audio_buffer(const audio_buffer&) = delete;
    audio_buffer& operator=(const audio_buffer&) = delete;

public:
    audio_buffer(std::size_t num_blocks, std::size_t block_size)
        : d_blocks(num_blocks)
        , d_current(0)
    {
        for (auto& block : d_blocks) {
            block.data.resize(block_size, 0);
            block.header.dwBufferLength = block.data.size() * sizeof(short);
            block.header.lpData = (LPSTR)block.data.data();
		}
    }

    WAVEHDR& current_header() {
        return d_blocks[d_current].header;
    }

    std::span<short> current_data() {
        return d_blocks[d_current].data;
    }

    void advance() {
        ++d_current;
        d_current %= d_blocks.size();
    }
};

}
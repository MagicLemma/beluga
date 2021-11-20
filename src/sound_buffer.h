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
        WAVEHDR            header;
        std::vector<short> data;
    };

    struct block_proxy
    {
        WAVEHDR*         header;
        std::span<short> data;

        void send_to_sound_device(HWAVEOUT device) const
        {
            // Prepare block for processing
			if (header->dwFlags & WHDR_PREPARED) {
				waveOutUnprepareHeader(device, header, sizeof(WAVEHDR));
            }

            waveOutPrepareHeader(device, header, sizeof(WAVEHDR));
			waveOutWrite(device, header, sizeof(WAVEHDR));
        }
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
            block.header.dwBufferLength = static_cast<DWORD>(block.data.size() * sizeof(short));
            block.header.lpData = (LPSTR)block.data.data();
		}
    }

    block_proxy next_block()
    {
        // Get the next block to use
        auto& current = d_blocks[d_current];
        ++d_current;
        d_current %= d_blocks.size();

        return {
            .header = &current.header,
            .data = current.data
        };
    }
};

}
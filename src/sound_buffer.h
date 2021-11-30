#pragma once
#include <Windows.h>

#include <array>
#include <vector>
#include <span>

namespace blga {

template <std::size_t NumBlocks, std::size_t SamplesPerBlock>
class audio_buffer
{
    struct block
    {
        WAVEHDR                            header;
        std::array<short, SamplesPerBlock> data;

        block() = default;

        block(const block&) = delete;
        block& operator=(const block&) = delete;

        void send_to_sound_device(HWAVEOUT device)
        {
            if (header.dwFlags & WHDR_PREPARED) {
                waveOutUnprepareHeader(device, &header, sizeof(WAVEHDR));
            }
            waveOutPrepareHeader(device, &header, sizeof(WAVEHDR));
            waveOutWrite(device, &header, sizeof(WAVEHDR));
        }
    };

    std::array<block, NumBlocks> d_blocks;
    std::size_t                  d_current;

public:
    audio_buffer()
        : d_blocks{}
        , d_current{0}
    {
        for (auto& block : d_blocks) {
            block.header.dwBufferLength = (DWORD)(sizeof(short) * SamplesPerBlock);
            block.header.lpData = (LPSTR)block.data.data();
        }
    }

    block& next_block()
    {
        d_current = (d_current + 1) % d_blocks.size();
        return d_blocks[d_current];
    }
};

}
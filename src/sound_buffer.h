#pragma once
#include <Windows.h>

#include <array>
#include <vector>
#include <span>

namespace blga {

struct block_proxy
{
    WAVEHDR*         header;
    std::span<short> data;

    void send_to_sound_device(HWAVEOUT device) const;
};

template <std::size_t NumBlocks, std::size_t SamplesPerBlock>
class audio_buffer
{
    struct block
    {
        WAVEHDR                            header;
        std::array<short, SamplesPerBlock> data;
    };

    std::array<block, NumBlocks> d_blocks;
    std::size_t                  d_current;
  
    audio_buffer(const audio_buffer&) = delete;
    audio_buffer& operator=(const audio_buffer&) = delete;

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

    block_proxy next_block()
    {
        auto& current = d_blocks[d_current++];
        d_current %= d_blocks.size();

        return {
            .header = &current.header,
            .data = current.data
        };
    }
};

}
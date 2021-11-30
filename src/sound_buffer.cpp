#include "sound_buffer.h"

#include <Windows.h>

namespace blga {

void block_proxy::send_to_sound_device(HWAVEOUT device) const
{
    if (header->dwFlags & WHDR_PREPARED) {
        waveOutUnprepareHeader(device, header, sizeof(WAVEHDR));
    }

    waveOutPrepareHeader(device, header, sizeof(WAVEHDR));
    waveOutWrite(device, header, sizeof(WAVEHDR));
}

audio_buffer::audio_buffer(std::size_t num_blocks, std::size_t samples_per_block)
    : d_blocks(num_blocks)
    , d_current(0)
{
    for (auto& block : d_blocks) {
        block.data.resize(samples_per_block, 0);
        block.header.dwBufferLength = (DWORD)(sizeof(short) * samples_per_block);
        block.header.lpData = (LPSTR)block.data.data();
    }
}

block_proxy audio_buffer::next_block()
{
    auto& current = d_blocks[d_current++];
    d_current %= d_blocks.size();

    return {
        .header = &current.header,
        .data = current.data
    };
}

}
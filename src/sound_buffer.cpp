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

}
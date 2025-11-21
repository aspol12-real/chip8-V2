#pragma once

#include <algorithm>
#include "ext/miniaudio.h"

class audioEngine {
    public:
        static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

};

class audioData {
    public:
        uint8_t audioBuffer[16];
        double phase = 0.0;           
        double targetFrequency = 500;  
        int sampleRate = 48000;
        float volume = 0.1f;
        bool sound_timer_active = false;
        bool muted = false;
};
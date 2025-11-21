#define MINIAUDIO_IMPLEMENTATION

#include "ext/miniaudio.h"

#include "sound.hpp"

void audioEngine::data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {

    //retrieve audio buffer
    audioData* data = (audioData*)pDevice->pUserData;
    if (data == NULL) return;

    float* pOutputF32 = (float*)pOutput;

    const double patternSize = 16.0;


    bool is_playing = data->sound_timer_active;


    if (!is_playing || data->targetFrequency == 0.0 || data->muted) {
        std::fill(pOutputF32, 
                  pOutputF32 + (frameCount * 2), 
                  0.0f);
        return;
    }

    double phaseIncrement = (patternSize * data->targetFrequency) / (double)data->sampleRate;

    for (ma_uint32 i = 0; i < frameCount; i++) {
        
        int index = (int)data->phase; 
        
        uint8_t sampleValue = data->audioBuffer[index];

        float normalizedSample = (sampleValue / 255.0f) * 2.0f - 1.0f;
        float finalSample = normalizedSample * data->volume;

        *pOutputF32++ = finalSample; 
        *pOutputF32++ = finalSample;

        data->phase += phaseIncrement;


        while (data->phase >= patternSize) {
            data->phase -= patternSize;
        }
    }
}

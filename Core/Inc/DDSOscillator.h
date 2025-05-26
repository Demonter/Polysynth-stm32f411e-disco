#ifndef DDSOSCILLATOR_H
#define DDSOSCILLATOR_H

#include <cstdint>

enum class Waveform {
    SINE,
    TRIANGLE,
    SAW,
    SQUARE
};

class DDSOscillator {
public:
    void setFrequency(float freq);
    void setWaveform(Waveform wf);
    int16_t nextSample();
    int16_t nextStereoSample(); // For I2S stereo transmission (duplicate channels)
    void setSampleRate(float rate);

private:
    static constexpr uint32_t LUT_SIZE = 256;
    static const int16_t sineLUT[LUT_SIZE];
    static const int16_t triangleLUT[LUT_SIZE];
    static const int16_t sawLUT[LUT_SIZE];

    uint32_t phaseAccumulator = 0;
    uint32_t phaseIncrement = 0;
    Waveform waveform = Waveform::SINE;

    float sampleRate = 48000.0f;  // Default value, can be overridden
    float frequency = 440.0f;
    uint32_t phase = 0;

    void updatePhaseIncrement();

};

#endif

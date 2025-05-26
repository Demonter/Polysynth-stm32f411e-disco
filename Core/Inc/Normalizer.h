#ifndef NORMALIZER_H
#define NORMALIZER_H

#include <cstdint>

class Normalizer {
public:
    explicit Normalizer(float gain = 1.0f);

    void setGain(float newGain);                 // Instant gain
    void setTargetGain(float target);            // For smoothed mode
    int16_t normalize(float sample);             // Output to DAC
    void enableSmoothing(bool enabled);          // Toggle mode

private:
    float gain;
    float smoothedGain;
    float smoothingFactor;
    bool smoothingEnabled;
};

#endif

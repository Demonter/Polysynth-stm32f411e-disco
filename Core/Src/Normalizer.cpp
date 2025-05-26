#include "Normalizer.h"
#include <algorithm>  // for std::clamp

Normalizer::Normalizer(float gain)
    : gain(gain), smoothedGain(gain), smoothingFactor(0.05f), smoothingEnabled(false) {}

void Normalizer::setGain(float newGain) {
    gain = newGain;
    if (!smoothingEnabled) {
        smoothedGain = gain;
    }
}

void Normalizer::setTargetGain(float target) {
    if (smoothingEnabled) {
        smoothedGain += smoothingFactor * (target - smoothedGain);
    } else {
        setGain(target);
    }
}

void Normalizer::enableSmoothing(bool enabled) {
    smoothingEnabled = enabled;
    if (!enabled) {
        smoothedGain = gain;
    }
}

int16_t Normalizer::normalize(float sample) {
//    float scaled = sample * smoothedGain;
	float scaled = sample * std::min(smoothedGain, 1.0f);  // never boost
    float clamped = std::clamp(scaled, -1.0f, 1.0f);
    return static_cast<int16_t>(clamped * 32767.0f);
}

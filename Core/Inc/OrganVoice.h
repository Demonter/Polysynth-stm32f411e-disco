#ifndef ORGANVOICE_H_
#define ORGANVOICE_H_

#include "DDSOscillator.h"
#include "ADSR.h"
#include <array>
#include <cstdint>
#include <cmath>

// Structure to define a preset for organ voice
struct OrganPreset {
    Waveform waveform = Waveform::SINE;
    std::array<float, 8> harmonicLevels {}; // harmonicLevels[0] = 1st harmonic, up to 8
    bool enableVibrato = false;
    float vibratoRate = 5.0f;    // Hz
    float vibratoDepth = 0.005f; // Fraction of base frequency
    bool enablePercussion = false;
    uint8_t percussionHarmonic = 2; // e.g., 2nd harmonic has percussion
    float percussionDecay = 0.1f;   // Seconds
    ADSR envelope; // Standard envelope
};

// Organ-style voice with multiple harmonics
class OrganVoice {
public:
    OrganVoice(float sampleRate, const OrganPreset& preset);

    void trigger(uint8_t note, uint8_t velocity);
    void release();
    float update();  // Call this every sample

    bool isActive() const;

private:
    float sampleRate;
    OrganPreset preset;

    std::array<DDSOscillator, 8> oscillators;
    std::array<float, 8> currentFreqs;
    ADSR envelope;

    bool active = false;
    uint8_t note = 0;
    float baseFrequency = 0.0f;

    float midiNoteToFreq(uint8_t note);
};

#endif

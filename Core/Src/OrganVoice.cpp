#include "OrganVoice.h"

OrganVoice::OrganVoice(float sampleRate, const OrganPreset& preset)
    : sampleRate(sampleRate), preset(preset), envelope(preset.envelope) {
    // Initialize frequencies to zero
    currentFreqs.fill(0.0f);
}

float OrganVoice::midiNoteToFreq(uint8_t note) {
    return 440.0f * powf(2.0f, (note - 69) / 12.0f);
}

void OrganVoice::trigger(uint8_t note, uint8_t velocity) {
    this->note = note;
    baseFrequency = midiNoteToFreq(note);
    active = true;
    envelope.trigger();

    // Initialize each harmonic oscillator
    for (size_t i = 0; i < oscillators.size(); ++i) {
        float harmonicFreq = baseFrequency * (i + 1);
        currentFreqs[i] = harmonicFreq;
        oscillators[i].setSampleRate(sampleRate);
        oscillators[i].setWaveform(preset.waveform);
        oscillators[i].setFrequency(harmonicFreq);
    }
}

void OrganVoice::release() {
    envelope.release();
}

float OrganVoice::update() {
    if (!active) return 0.0f;

    float envLevel = envelope.nextSample();
    float output = 0.0f;

    for (size_t i = 0; i < oscillators.size(); ++i) {
        float sample = oscillators[i].nextSample();
        output += sample * preset.harmonicLevels[i];
    }

    output *= envLevel;

    if (!envelope.isActive()) {
        active = false;
    }

    return output;
}

bool OrganVoice::isActive() const {
    return active;
}

#include "Voice.h"


void Voice::setEnvelope(float attack, float decay, float sustain, float release) {
    envelope.setAttackTime(attack);
    envelope.setDecayTime(decay);
    envelope.setSustainLevel(sustain);
    envelope.setReleaseTime(release);
}

void Voice::trigger(uint8_t midiNote, uint8_t velocity, Waveform waveform) {
    note = midiNote;
    oscillator.setFrequency(midiNoteFreqTable[midiNote]);
    oscillator.setWaveform(waveform);
    envelope.trigger();
    active = true;
}

void Voice::release() {
    envelope.release();
}

bool Voice::isActive() const {
    return active;
}

uint8_t Voice::getNote() const {
    return note;
}

bool Voice::isPlayingNote(uint8_t n) const {
    return active && (note == n);
}

float Voice::nextSample() {
    if (!active) return 0.0f;

    float sample = oscillator.nextSample();
    float env = envelope.nextSample();
    float output = sample * env;

    if (!envelope.isActive()) {
        active = false;
    }

    return output;
}

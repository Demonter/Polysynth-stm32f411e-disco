#include "ADSR.h"

ADSR::ADSR() {}

void ADSR::setSampleRate(float rate) {
    sampleRate = rate;
    setAttackTime(attackTime);
    setDecayTime(decayTime);
    setReleaseTime(releaseTime);
}

void ADSR::setAttackTime(float seconds) {
    attackTime = seconds;
    attackRate = (seconds > 0.0f) ? (1.0f / (seconds * sampleRate)) : 1.0f;
}

void ADSR::setDecayTime(float seconds) {
    decayTime = seconds;
    decayRate = (seconds > 0.0f) ? ((1.0f - sustainLevel) / (seconds * sampleRate)) : 1.0f;
}

void ADSR::setSustainLevel(float level_) {
    sustainLevel = level_;
    setDecayTime(decayTime); // recalc decayRate
}

void ADSR::setReleaseTime(float seconds) {
    releaseTime = seconds;
    releaseRate = (seconds > 0.0f) ? (sustainLevel / (seconds * sampleRate)) : 1.0f;
}

void ADSR::trigger() {
    state = State::Attack;
}

void ADSR::release() {
    state = State::Release;
}

void ADSR::reset() {
    state = State::Idle;
    level = 0.0f;
}

float ADSR::nextSample() {
    switch (state) {
        case State::Idle:
            return 0.0f;

        case State::Attack:
            level += attackRate;
            if (level >= 1.0f) {
                level = 1.0f;
                state = State::Decay;
            }
            break;

        case State::Decay:
            level -= decayRate;
            if (level <= sustainLevel) {
                level = sustainLevel;
                state = State::Sustain;
            }
            break;

        case State::Sustain:
            // Hold sustain level
            break;

        case State::Release:
            level -= releaseRate;
            if (level <= 0.0f) {
                level = 0.0f;
                state = State::Idle;
            }
            break;
    }

    return level;
}

bool ADSR::isActive() const {
    return state != State::Idle;
}

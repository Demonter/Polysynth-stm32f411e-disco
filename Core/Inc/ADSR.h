#pragma once

class ADSR {
public:
    enum class State {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release
    };

    ADSR();

    void setSampleRate(float rate);
    void setAttackTime(float seconds);
    void setDecayTime(float seconds);
    void setSustainLevel(float level); // 0.0–1.0
    void setReleaseTime(float seconds);

    void trigger(); // Starts envelope (Attack)
    void release(); // Starts Release phase
    void reset();   // Immediately goes to Idle

    float nextSample(); // Get the current envelope value
    bool isActive() const;

private:
    State state = State::Idle;

    float sampleRate = 48000.0f;

    float attackRate = 0.0f;
    float decayRate = 0.0f;
    float releaseRate = 0.0f;

    float attackTime = 0.01f;
    float decayTime = 0.1f;
    float sustainLevel = 0.7f;
    float releaseTime = 0.2f;

    float level = 0.0f;
};

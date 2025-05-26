#ifndef ADSRENVELOPE_H
#define ADSRENVELOPE_H

#include <cstdint>

class AdsrEnvelope {
public:
    void trigger();
    void release();
    float getAmplitude();

private:
    enum State { IDLE, ATTACK, DECAY, SUSTAIN, RELEASE } state = IDLE;
    float amplitude = 0.0f;
};

#endif

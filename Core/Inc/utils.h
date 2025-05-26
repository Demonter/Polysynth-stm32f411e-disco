#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include "DDSOscillator.h"

static inline const char* waveformToString(Waveform wf) {
    switch (wf) {
        case Waveform::SINE:     return "SINE";
        case Waveform::TRIANGLE: return "TRIANGLE";
        case Waveform::SAW:      return "SAWTOOTH";
        case Waveform::SQUARE:   return "SQUARE";
        default:                 return "UNKNOWN";
    }
}

#endif

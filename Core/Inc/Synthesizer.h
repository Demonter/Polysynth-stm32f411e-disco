#ifndef SYNTHESIZER_H
#define SYNTHESIZER_H

#include "Voice.h"
#include "MidiInterface.h"
#include "Cs43l22Driver.h"
#include "Normalizer.h"

class Synthesizer {
public:
    Synthesizer();
    virtual ~Synthesizer();

    void init();
    void update();

    void noteOn(uint8_t note, uint8_t velocity);
    void noteOff(uint8_t note);
    void fillFirstHalf();
    void fillSecondHalf();

    void cycleWaveform();
    Waveform getCurrentWaveform() const;

private:
    void startAudioDMA();

    static constexpr int MAX_VOICES = 8;
    Voice voices[MAX_VOICES];
    int nextVoiceIndex = 0;

    MidiInterface midi;
    Cs43l22Driver* codec;
    Normalizer normalizer = Normalizer(1.0f / MAX_VOICES);

    static constexpr size_t AUDIO_BUFFER_SIZE = 512;
    int16_t audioBuffer[AUDIO_BUFFER_SIZE];
    void fillBufferRange(size_t offset);

    Waveform currentWaveform = Waveform::SINE;
};

#endif

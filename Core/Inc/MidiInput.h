#ifndef MIDI_INPUT_H
#define MIDI_INPUT_H

#include "stm32f4xx_hal.h"

bool midiQueueAvailable();
void midiQueuePush(uint8_t byte);
uint8_t midiQueuePop();

enum class MidiMessageType {
    None,
    NoteOn,
    NoteOff,
    Other
};

struct MidiMessage {
    MidiMessageType type;
    uint8_t channel;
    uint8_t note;
    uint8_t velocity;
};

class MidiInput {
public:
    MidiInput(UART_HandleTypeDef* uart);
    void init();                          // Start reception
    void handleByte(uint8_t byte);        // Feed new byte
    void process();                       // Optional polling mode
    MidiMessage getLastMessage();         // Optional: expose last message
    uint8_t* getRxBytePtr();
    void processQueue();
    uint8_t getRxByte() const;
    void midiQueuePush(uint8_t byte);
    void processDma();

private:
    UART_HandleTypeDef* huart;
    uint8_t rxByte;
    uint8_t status;
    uint8_t data1;
    bool waitingData1;
    MidiMessage lastMessage;
};

#endif

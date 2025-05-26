#ifndef MIDIINTERFACE_H
#define MIDIINTERFACE_H

#include <cstdint>

class MidiInterface {
public:
    void init();
    bool readMessage(uint8_t& status, uint8_t& data1, uint8_t& data2);
};

#endif

#include "MidiInput.h"
#include "MidiDMA.h"
#include <cstring>
#include <cstdio>
#include "Synthesizer.h"
#include "LogBuffer.h"

extern Synthesizer synth;
extern UART_HandleTypeDef huart2;

#define MIDI_QUEUE_SIZE 64

static volatile uint8_t midiQueue[MIDI_QUEUE_SIZE];
static volatile uint8_t midiHead = 0;
static volatile uint8_t midiTail = 0;

bool midiQueueAvailable() {
    return midiHead != midiTail;
}

void midiQueuePush(uint8_t byte) {
    uint8_t next = (midiHead + 1) % MIDI_QUEUE_SIZE;
    if (next != midiTail) {
        midiQueue[midiHead] = byte;
        midiHead = next;
    }
    // else: overflow, silently drop or set error flag
}

uint8_t midiQueuePop() {
    uint8_t byte = midiQueue[midiTail];
    midiTail = (midiTail + 1) % MIDI_QUEUE_SIZE;
    return byte;
}

MidiInput::MidiInput(UART_HandleTypeDef* uart)
    : huart(uart), status(0), data1(0), waitingData1(false)
{
    memset(&lastMessage, 0, sizeof(lastMessage));
}

void MidiInput::init() {
	MidiDMA_Start(huart);
//    HAL_UART_Receive_IT(huart, &rxByte, 1);
}

void MidiInput::handleByte(uint8_t byte) {
	if (byte & 0x80) {
		status = byte;
		waitingData1 = true;
	} else {
		if (waitingData1) {
			data1 = byte;
			waitingData1 = false;
		} else {
			// Parse full message
			lastMessage.channel = status & 0x0F;
			lastMessage.note = data1;
			lastMessage.velocity = byte;

			uint8_t cmd = status & 0xF0;
			if (cmd == 0x90 && byte > 0) {
				lastMessage.type = MidiMessageType::NoteOn;
				synth.noteOn(lastMessage.note, lastMessage.velocity);
				LogBuffer::info("Note ON: ch %d, note %d, vel %d\r\n", lastMessage.channel, lastMessage.note, lastMessage.velocity);
			} else if (cmd == 0x80 || (cmd == 0x90 && byte == 0)) {
				lastMessage.type = MidiMessageType::NoteOff;

				synth.noteOff(lastMessage.note);

				LogBuffer::info("Note OFF: ch %d note %d\r\n", lastMessage.channel, lastMessage.note);
			} else {
				lastMessage.type = MidiMessageType::Other;
			}

			waitingData1 = true;
		}
	}
}

MidiMessage MidiInput::getLastMessage() {
    return lastMessage;
}

uint8_t* MidiInput::getRxBytePtr() {
    return &rxByte;
}

void MidiInput::processQueue() {
    while (midiQueueAvailable()) {
        uint8_t byte = midiQueuePop();
        handleByte(byte);  // Existing MIDI parsing logic
    }
}

uint8_t MidiInput::getRxByte() const {
    return rxByte;
}

void MidiInput::processDma() {
    static size_t lastPos = 0;
    size_t pos = MIDI_DMA_BUF_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);
    while (lastPos != pos) {
        uint8_t b = midiDmaBuf[lastPos];
        lastPos = (lastPos + 1) % MIDI_DMA_BUF_SIZE;
        handleByte(b);  // Existing MIDI parser
    }
}

#include "MidiDMA.h"
#include "LogBuffer.h"

uint8_t midiDmaBuf[MIDI_DMA_BUF_SIZE];

static UART_HandleTypeDef* uart = nullptr;

static size_t lastPos = 0;

void MidiDMA_Start(UART_HandleTypeDef* huart) {
    uart = huart;
    HAL_UART_Receive_DMA(uart, midiDmaBuf, MIDI_DMA_BUF_SIZE);
}

void MidiDMA_Process(void (*handler)(uint8_t)) {
    if (!uart || !uart->hdmarx) return;

    size_t pos = MIDI_DMA_BUF_SIZE - __HAL_DMA_GET_COUNTER(uart->hdmarx);
    LogBuffer::info("DMA Pos: %d, LastPos: %d\r\n", pos, lastPos);
    while (lastPos != pos) {
        handler(midiDmaBuf[lastPos]);
        lastPos = (lastPos + 1) % MIDI_DMA_BUF_SIZE;
    }
}

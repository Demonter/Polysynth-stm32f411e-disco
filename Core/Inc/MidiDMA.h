#ifndef MIDI_DMA_H
#define MIDI_DMA_H

#include "stm32f4xx_hal.h"
#define MIDI_DMA_BUF_SIZE 128

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t midiDmaBuf[MIDI_DMA_BUF_SIZE];  // expose buffer to other files

void MidiDMA_Start(UART_HandleTypeDef* huart);
void MidiDMA_Process(void (*handler)(uint8_t));

#ifdef __cplusplus
}
#endif

#endif // MIDI_DMA_H

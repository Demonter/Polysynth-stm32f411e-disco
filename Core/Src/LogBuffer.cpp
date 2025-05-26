#include "LogBuffer.h"
#include <cstring>
#include <cstdio>
#include <cstdarg>

char LogBuffer::buffer[BUF_SIZE];
char LogBuffer::dmaActiveBuffer[BUF_SIZE];
size_t LogBuffer::dmaLength = 0;

volatile size_t LogBuffer::head = 0;
volatile size_t LogBuffer::tail = 0;
UART_HandleTypeDef* LogBuffer::uart = nullptr;
bool LogBuffer::dmaBusy = false;
LogLevel LogBuffer::currentLevel = LogLevel::Debug;

void LogBuffer::init(UART_HandleTypeDef* u, LogLevel level) {
    uart = u;
    currentLevel = level;
    head = tail = 0;
    dmaBusy = false;
}

void LogBuffer::setLevel(LogLevel level) {
    currentLevel = level;
}

void LogBuffer::write(const char* msg) {
    while (*msg) {
        size_t next = (head + 1) % BUF_SIZE;
        if (next == tail) break;  // overflow: drop char
        buffer[head] = *msg++;
        head = next;
    }
    tryTransmit();
}

void LogBuffer::vprint(LogLevel level, const char* fmt, va_list args) {
    if (level < currentLevel) return;

    char temp[128];
    vsnprintf(temp, sizeof(temp), fmt, args);
    write(temp);
}

void LogBuffer::info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprint(LogLevel::Info, fmt, args);
    va_end(args);
}

void LogBuffer::warn(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprint(LogLevel::Warn, fmt, args);
    va_end(args);
}

void LogBuffer::error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprint(LogLevel::Error, fmt, args);
    va_end(args);
}

void LogBuffer::debug(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprint(LogLevel::Debug, fmt, args);
    va_end(args);
}

void LogBuffer::tryTransmit() {
    if (dmaBusy || uart == nullptr || head == tail) return;

    size_t i = 0;
    while (tail != head && i < BUF_SIZE - 1) {
        dmaActiveBuffer[i++] = buffer[tail];
        tail = (tail + 1) % BUF_SIZE;
    }
    dmaActiveBuffer[i] = '\0';
    dmaLength = i;

    if (dmaLength > 0) {
        // Toggle LED or set pin high here for scope
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);  // Example: onboard LED
        HAL_UART_Transmit_DMA(uart, (uint8_t*)dmaActiveBuffer, dmaLength);
        dmaBusy = true;
    }
}

void LogBuffer::dmaCallback() {
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);  // Toggle LED on DMA complete
    dmaBusy = false;
    tryTransmit(); // keep draining buffer
}

void LogBuffer::process() {
    tryTransmit(); // optional fallback
}

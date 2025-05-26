#ifndef LOGBUFFER_H
#define LOGBUFFER_H

#include "stm32f4xx_hal.h"
#include <cstddef>
#include <cstdint>
#include <cstdarg>

enum class LogLevel {
    Debug,
    Info,
    Warn,
    Error
};

class LogBuffer {
public:
    static void init(UART_HandleTypeDef* huart, LogLevel level = LogLevel::Debug);
    static void setLevel(LogLevel level);
    static void info(const char* fmt, ...);
    static void warn(const char* fmt, ...);
    static void error(const char* fmt, ...);
    static void debug(const char* fmt, ...);
    static void process(); // Optional manual flushing
    static void tryTransmit();

    static bool dmaBusy;

private:
    static constexpr size_t BUF_SIZE = 1024;

    static char buffer[BUF_SIZE];
    static volatile size_t head;
    static volatile size_t tail;
    static UART_HandleTypeDef* uart;
    static LogLevel currentLevel;

    static char dmaActiveBuffer[BUF_SIZE];   // DMA transfer buffer
    static size_t dmaLength;

    static void write(const char* msg);
    static void vprint(LogLevel level, const char* fmt, va_list args);
    static void dmaCallback();

    friend void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart);
};

#endif // LOGBUFFER_H

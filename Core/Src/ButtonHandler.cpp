#include "ButtonHandler.h"
#include "stm32f4xx_hal.h"

extern uint32_t HAL_GetTick();

ButtonHandler::ButtonHandler(GPIO_TypeDef* port, uint16_t pin, uint32_t debounceTimeMs)
    : port(port), pin(pin), debounceTime(debounceTimeMs), lastPressTime(0), pressDetected(false) {}

void ButtonHandler::onInterrupt() {
    uint32_t now = HAL_GetTick();
    if ((now - lastPressTime) >= debounceTime) {
        pressDetected = true;
        lastPressTime = now;
    }
}

void ButtonHandler::update() {
    // Could add future functionality (e.g., hold time)
}

bool ButtonHandler::wasPressed() {
    if (pressDetected) {
        pressDetected = false;
        return true;
    }
    return false;
}

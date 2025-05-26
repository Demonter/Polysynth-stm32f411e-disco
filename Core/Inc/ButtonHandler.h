#ifndef BUTTONHANDLER_H_
#define BUTTONHANDLER_H_

#include "stm32f4xx_hal.h"

class ButtonHandler {
public:
    ButtonHandler(GPIO_TypeDef* port, uint16_t pin, uint32_t debounceTimeMs = 200);

    void onInterrupt();        // Call in EXTI IRQ handler
    void update();             // Call periodically in main loop
    bool wasPressed();         // Returns true once per press

private:
    GPIO_TypeDef* port;
    uint16_t pin;
    uint32_t debounceTime;
    uint32_t lastPressTime;
    volatile bool pressDetected;
};

#endif

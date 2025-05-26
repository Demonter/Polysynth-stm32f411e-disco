#ifndef CS43L22DRIVER_H
#define CS43L22DRIVER_H

#include "stm32f4xx_hal.h"

class Cs43l22Driver {
public:
    Cs43l22Driver(I2C_HandleTypeDef* i2cHandle);

    bool init();
    bool powerOn();
    bool setVolume(uint8_t volume); // 0-100%
    bool enableHeadphones();
    bool enableSpeaker();
    bool disableOutput();

private:
    bool writeRegister(uint8_t reg, uint8_t value);
    I2C_HandleTypeDef* hi2c;
    static constexpr uint8_t CS43L22_ADDRESS = 0x94; // 7-bit addr shifted left
};

#endif

#include "Cs43l22Driver.h"
#include <cstring>

Cs43l22Driver::Cs43l22Driver(I2C_HandleTypeDef* i2cHandle) : hi2c(i2cHandle) {}

bool Cs43l22Driver::writeRegister(uint8_t reg, uint8_t value) {
    return HAL_I2C_Mem_Write(hi2c, CS43L22_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, HAL_MAX_DELAY) == HAL_OK;
}

bool Cs43l22Driver::init() {
    // Required magic init
    writeRegister(0x00, 0x99);
    writeRegister(0x47, 0x80);
    writeRegister(0x32, 0x80);
    writeRegister(0x32, 0x00);
    writeRegister(0x00, 0x00);

    // Power control 1: speaker & headphone off
    writeRegister(0x04, 0x03);  // Chip powered ON, no output yet

    // Clocking control: Auto detect clock
    writeRegister(0x05, 0x81);  // I2S, 16-bit
    writeRegister(0x06, 0x07);  // MCLK/8
    writeRegister(0x0D, 0x00);  // Auto clock detect

    // Passthrough / mixer off
    writeRegister(0x0A, 0x00);  // Disable passthrough A
    writeRegister(0x27, 0x00);  // Analog ZC disable
    writeRegister(0x1A, 0x00);  // Left volume = 0 dB
    writeRegister(0x1B, 0x00);  // Right volume = 0 dB

    // Power control 2: turn on HP
    writeRegister(0x02, 0x9E);  // Power on all + HP A/B enable

    return true;
}

bool Cs43l22Driver::powerOn() {
//    return writeRegister(0x02, 0x9E); // power up all
	return true;
}

bool Cs43l22Driver::setVolume(uint8_t volume) {
    uint8_t vol = static_cast<uint8_t>(255 - (volume * 2.55f));
    return writeRegister(0x1A, vol) && writeRegister(0x1B, vol);
}

bool Cs43l22Driver::enableHeadphones() {
    return writeRegister(0x04, 0xAF); // Enable headphone out
}

bool Cs43l22Driver::enableSpeaker() {
    return writeRegister(0x04, 0xA0); // Enable speaker out
}

bool Cs43l22Driver::disableOutput() {
    return writeRegister(0x04, 0xFF); // Power down all
}


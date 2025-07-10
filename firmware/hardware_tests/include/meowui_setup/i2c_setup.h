#pragma once
#include <stdint.h>

#define PIN_I2C_SDA (uint8_t)2
#define PIN_I2C_SCL (uint8_t)1
#define PIN_I2C_INT (uint8_t)42

#define I2C_AWAIT_TIME_MS (unsigned long)100 // Час очікування відповіді від i2c пристрою

enum EXT_I2C_CMD : uint8_t // Перечислення містить список команд для керування іншими МК по шині I2C.
{
    I2C_CMD_ENABLE = 0,
    I2C_CMD_DISABLE,
    I2C_CMD_PIN_ON,
    I2C_CMD_PIN_OFF,
    I2C_CMD_BTNS_STATE,
};
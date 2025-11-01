#pragma once
#include <stdint.h>

#define PIN_I2C_SDA (uint8_t)2
#define PIN_I2C_SCL (uint8_t)1
#define PIN_I2C_INT (uint8_t)42

#define I2C_AWAIT_TIME_MS (unsigned long)100  // Час очікування відповіді від i2c пристрою

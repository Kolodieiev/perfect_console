#pragma once
#include <stdint.h>

#if defined(CONFIG_IDF_TARGET_ESP32S3)

#define PIN_I2S_OUT_BCLK 21
#define PIN_I2S_OUT_LRC 48
#define PIN_I2S_OUT_DOUT 47

#define PIN_I2S_IN_SCK 3
#define PIN_I2S_IN_WS 46
#define PIN_I2S_IN_SD 9

#else

#define PIN_I2S_OUT_LRC (uint8_t)15
#define PIN_I2S_OUT_DOUT (uint8_t)5
#define PIN_I2S_OUT_BCLK (uint8_t)18

#define PIN_I2S_IN_SD (uint8_t)14
#define PIN_I2S_IN_SCK (uint8_t)27
#define PIN_I2S_IN_WS (uint8_t)26

#endif
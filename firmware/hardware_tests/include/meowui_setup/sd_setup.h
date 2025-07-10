#pragma once
#include <stdint.h>

#define SD_SPI_BUS HSPI // Номер шини SPI

#define SD_PIN_CS (int8_t)10
#define SD_PIN_MOSI (int8_t)11
#define SD_PIN_SCLK (int8_t)12
#define SD_PIN_MISO (int8_t)13

#define SD_FREQUENCY 80000000
#define SD_MOUNTPOINT "/sd" // Не міняй!
#define SD_MAX_FILES (uint8_t)255

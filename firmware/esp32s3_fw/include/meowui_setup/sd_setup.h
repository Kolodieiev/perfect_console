#pragma once
#include <stdint.h>

#define SD_SPI_BUS VSPI // Номер шини SPI

#define SD_PIN_CS 10 // Бібліотека карти пам'яті вимагає реальний пін, навіть у випадку, якщо CS приєдано до землі. 
#define SD_PIN_MOSI 14
#define SD_PIN_SCLK 38
#define SD_PIN_MISO 39

#define SD_FREQUENCY 80000000
#define SD_MOUNTPOINT "/sd" // Не міняй!
#define SD_MAX_FILES 255

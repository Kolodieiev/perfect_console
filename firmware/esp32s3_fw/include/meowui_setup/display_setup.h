#pragma once
#include <stdint.h>

#define DISABLE_ALL_LIBRARY_WARNINGS
#define USER_SETUP_ID 304
#define ST7789_DRIVER
#define TFT_RGB_ORDER TFT_BGR
#define SPI_FREQUENCY 80000000

#define TFT_WIDTH 240
#define TFT_HEIGHT 320

#define LOAD_FONT2
#define LOAD_FONT4

#define TFT_CUTOUT 0

#define USE_HSPI_PORT
//
#define TFT_MOSI 11 // sda
#define TFT_SCLK 12 // scl
#define TFT_MISO 13
#define TFT_CS -1

#define TFT_DC 18
#define TFT_RST 17

#define BACKLIGHT_PIN 8
#define PWM_FREQ 3000
#define PWM_RESOLUTION 8

#pragma once

#define BUSS_FREQUENCY 80000000  // Частота шини дисплею.

#define TFT_WIDTH 240   // Ширина дисплею.
#define TFT_HEIGHT 320  // Висота дисплею.

#define SPI_PORT HSPI  // Порт, на якому працюватиме шина SPI дисплею.
//
#define TFT_MOSI 11  // sda.
#define TFT_SCLK 12  // scl.
#define TFT_MISO 13  // Навіть якщо на дисплеї відсутній цей пін, його не можна ніде використовувати для коректної роботи шини SPI.
#define TFT_RST 17   // TFT reset pin connect to RST.
#define TFT_DC 18
#define TFT_CS -1

#define BACKLIGHT_PIN 8  // Закоментуй, якщо відсутній пін управління підсвіткою дисплея.
#define HAS_BL_PWM       // Закоментуй, якщо відсутнє управління яскравістю підсвітки дисплея.
#define PWM_FREQ 3000    // Частота PWM підсвітки дисплея.
#define PWM_RESOLUTION 8

#define DISPLAY_ROTATION 0  // Стартова орієнтація дисплея.

#define IS_IPS_DISPLAY true  // Тип матриці дисплея.

#define BUS_TYPE Arduino_ESP32SPI                                                         // Клас шини.
#define IS_COMMON_BUS false                                                               // Парапор, який вказує чи є шина спільною для декількох пристроїв.
#define BUS_PARAMS TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO, SPI_PORT, IS_COMMON_BUS  // Параметри класу шини.

#define DISP_DRIVER_TYPE Arduino_ST7789                                                      // Клас драйвера дисплея.
#define DISP_DRIVER_PARAMS TFT_RST, DISPLAY_ROTATION, IS_IPS_DISPLAY, TFT_WIDTH, TFT_HEIGHT  // Параметри класу драйвера дисплея БЕЗ адреси шини.

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Нижче налаштування для JC3248W535C

// #define BUSS_FREQUENCY 60000000

// #define TFT_WIDTH 320  // Ширина дисплею.
// #define TFT_HEIGHT 480 // Висота дисплею.

// #define TFT_CS 45
// #define TFT_SCLK 47
// #define TFT_MOSI 21
// #define TFT_MISO 48
// #define TFT_D2 40
// #define TFT_D3 39
// #define TFT_DC 8
// #define TFT_RST -1

// #define BACKLIGHT_PIN 1 // Закоментуй, якщо відсутній пін управління підсвіткою дисплея.
// #define HAS_BL_PWM      // Закоментуй, якщо відсутнє управління яскравістю підсвітки дисплея.
// #define PWM_FREQ 3000   // Частота PWM підсвітки дисплея.
// #define PWM_RESOLUTION 8

// #define DISPLAY_ROTATION 0 // Стартова орієнтація дисплея.

// #define IS_IPS_DISPLAY true // Тип матриці дисплея.

// #define BUS_TYPE Arduino_ESP32QSPI                                                      // Клас шини.
// #define IS_COMMON_BUS false                                                            // Парапор, який вказує чи є шина спільною для декількох пристроїв.
// #define BUS_PARAMS TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO, TFT_D2, TFT_D3, IS_COMMON_BUS // Параметри класу шини.

// #define DISP_DRIVER_TYPE Arduino_AXS15231B                                                  // Клас драйвера дисплея.
// #define DISP_DRIVER_PARAMS TFT_RST, DISPLAY_ROTATION, IS_IPS_DISPLAY, TFT_WIDTH, TFT_HEIGHT // Параметри класу драйвера дисплея БЕЗ адреси шини.
/**
 * @file graphics_setup.h
 * @brief Файл налаштувань графічного драйвера
 * @details
 */

#pragma once

#define UI_UPDATE_DELAY 25  // Затримка (мс) між фреймами

#define GRAPHICS_ENABLED  // Увімкнути підтримку графічного драйвера. Закоментуй, якщо в проєкті не використовується дисплей.
// #define DIRECT_DRAWING       // Увімкнути пряме малювання на дисплей замість буферу. Закоментуй, для формування зображення через буфер.

#ifdef GRAPHICS_ENABLED

#ifndef DIRECT_DRAWING
#define DOUBLE_BUFFERRING  // Подвійна буферизація. Працює тільки за наявності PSRAM. Інакше буде викликано рестарт МК
// #define SHOW_FPS             // Відображати значення FPS на дисплеї
// #define ENABLE_SCREENSHOTER  // Увімкнути підтримку створення скриншотів. Тримай закоментованим, якщо не використовується
#endif  // #ifndef DIRECT_DRAWING

//-----------------------------------------------------------------------------------------------------------------------------

#define BUSS_FREQUENCY 80000000  // Частота шини дисплея.

#define UI_WIDTH   // Ширина дисплея(канвасу).
#define UI_HEIGHT  // Висота дисплея(канвасу).

#define UI_WIDTH 240   // Ширина UI.
#define UI_HEIGHT 320  // Висота UI.

#define ROTATE_CANVAS false  // Зазвичай не потрібно встановлювати в true. Можна спробувати, якщо нічого не допомогло.

#define DISPLAY_WIDTH 240   // Ширина дисплея.
#define DISPLAY_HEIGHT 320  // Висота дисплея.

#define SPI_PORT HSPI  // Порт, на якому працюватиме шина SPI дисплея.
//
#define TFT_MOSI 11  // sda.
#define TFT_SCLK 12  // scl.
#define TFT_MISO 13  // Навіть якщо на дисплеї відсутній цей пін, його не можна ніде використовувати для коректної роботи шини SPI.
#define TFT_RST 17   // Якщо пін підключено RST мікроконтролера, вказати -1
#define TFT_DC 18    //
#define TFT_CS -1    // Якщо на шині SPI знаходиться тільки дисплей(що рекомендовано), вказати -1

#define PIN_DISPLAY_BL 8          // Закоментуй, якщо відсутній пін управління підсвіткою дисплея.
#define HAS_BL_PWM                // Закоментуй, якщо відсутнє управління яскравістю підсвітки дисплея.
#define DISPLAY_BL_PWM_FREQ 3000  // Частота PWM підсвітки дисплея.
#define DISPLAY_BL_PWM_RES 8      //

#define DISPLAY_ROTATION 0  // Стартова орієнтація дисплея.

#define IS_IPS_DISPLAY true  // Тип матриці дисплея.
#define INVERT_COLORS false  // Чи потрібно інвертувати кольори пікселів.

#define BUS_TYPE Arduino_ESP32SPI                                                         // Клас шини.
#define IS_COMMON_BUS false                                                               // Парапор, який вказує чи є шина спільною для декількох пристроїв.
#define BUS_PARAMS TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO, SPI_PORT, IS_COMMON_BUS  // Параметри класу шини.

#define DISP_DRIVER_TYPE Arduino_ST7789                                                              // Клас драйвера дисплея.
#define DISP_DRIVER_PARAMS TFT_RST, DISPLAY_ROTATION, IS_IPS_DISPLAY, DISPLAY_WIDTH, DISPLAY_HEIGHT  // Параметри класу драйвера дисплея БЕЗ адреси шини.

#endif  // #ifdef GRAPHICS_ENABLED

//-----------------------------------------------------------------------------------------------------------------------------

// Нижче приклади налаштувань для інших дисплеїв

//-----------------------------------------------------------------------------------------------------------------------------

// Приклад налаштувань для MIPI DSI дисплея на платі ESP32p4
//-----------------------------------------------------------------------------------------------------------------------------

// #define GRAPHICS_ENABLED  // Увімкнути підтримку графічного драйвера. Закоментуй, якщо в проєкті не використовується дисплей.

// #define UI_UPDATE_DELAY 25  // Мінімальна затримка (мс) між фреймами(викликом update).

// #define UI_WIDTH 1024  // Ширина UI.
// #define UI_HEIGHT 600  // Висота UI.

// #define ROTATE_CANVAS false  // Зазвичай не потрібно встановлювати в true. Можна спробувати, якщо нічого не допомогло.

// #define DISPLAY_WIDTH 1024  // Ширина дисплея.
// #define DISPLAY_HEIGHT 600  // Висота дисплея.

// #ifdef GRAPHICS_ENABLED

// #define SHOW_FPS  // Відображати значення FPS на дисплеї.
// #define ENABLE_SCREENSHOTER  // Увімкнути підтримку створення скриншотів. Тримай закоментованим, якщо не використовується.

//-----------------------------------------------------------------------------------------------------------------------------

// #define LCD_DRIVER_EK79007  // Для якого драйвера буде застосовано команду ініціалізації дисплея.
// // #define LCD_DRIVER_ST7701
// // #define LCD_DRIVER_HX8394
// // #define LCD_DRIVER_JD9165
// // #define LCD_DRIVER_JD9365

// #define HSYNC_PULSE_WIDTH 10
// #define HSYNC_BACK_PORCH 160
// #define HSYNC_FRONT_PORCH 160

// #define VSYNC_PULSE_WIDTH 1
// #define VSYNC_BACK_PORCH 23
// #define VSYNC_FRONT_PORCH 12

// #define BUSS_FREQUENCY 60000000
// #define LANE_BITRATE 1200

// #define CLOCK_SOURCE MIPI_DSI_PHY_PLLREF_CLK_SRC_PLL_F20M  // Для ревізії чіпа P4 3+ Потрібно задати інше джерело тактування

// #define PIN_LCD_RESET 27

// #define PIN_DISPLAY_BL 26         // Закоментуй, якщо відсутній пін управління підсвіткою дисплея.
// #define HAS_BL_PWM                // Закоментуй, якщо відсутнє управління яскравістю підсвітки дисплея.
// #define DISPLAY_BL_PWM_FREQ 3000  // Частота PWM підсвітки дисплея.
// #define DISPLAY_BL_PWM_RES 8

// #define DISPLAY_ROTATION 0  // Стартова орієнтація дисплея.

// #define BUS_TYPE Arduino_ESP32DSIPanel                                                                                                                                         // Клас шини.
// #define BUS_PARAMS HSYNC_PULSE_WIDTH, HSYNC_BACK_PORCH, HSYNC_FRONT_PORCH, VSYNC_PULSE_WIDTH, VSYNC_BACK_PORCH, VSYNC_FRONT_PORCH, BUSS_FREQUENCY, LANE_BITRATE, CLOCK_SOURCE  // Параметри класу шини.

// #define DISP_DRIVER_TYPE Arduino_DSI_Display                                               // Клас драйвера дисплея.
// #define DISP_DRIVER_PARAMS DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_ROTATION, PIN_LCD_RESET  // Параметри класу драйвера дисплея БЕЗ адреси шини.

// #endif  // #ifdef GRAPHICS_ENABLED

//-----------------------------------------------------------------------------------------------------------------------------
#pragma once
#include <stdint.h>

#define EXT_INPUT // Якщо стан кнопок зчитується з іншого МК по шині I2C.

#ifdef EXT_INPUT
#define EXT_INPUT_B_NUM 1 // Кількість байт(які будуть прочитані по I2C) що містять інформацію про стан кнопок. 1 байт == 8 кнопок.
#endif

// #define TOUCHSCREEN_SUPPORT // Увімкнути підтримку сенсорного екрану

#ifdef TOUCHSCREEN_SUPPORT
#define TOUCH_ROTATION ROTATION_0
#define TOUCH_WIDTH 320
#define TOUCH_HEIGHT 480

// -------------------------------- Обрати драйвер сенсорного екрану
#define GT911_DRIVER // Поки що додано підтримку тільки 1 чіпа

// -------------------------------- Налаштуй піни, до яких підключено драйвер тачскріна
#ifdef GT911_DRIVER
#define TOUCH_SDA_PIN 33
#define TOUCH_SCL_PIN 32
#define TOUCH_INT_PIN 21
#define TOUCH_RST_PIN 25
#endif // GT911_DRIVER

#endif // TOUCHSCREEN_SUPPORT

// -------------------------------- Закріплення віртуальних назв кнопок до виводів МК
// Якщо стан кнопок зчитується з іншого МК, тоді ідентифікатором віртуальної кнопки має бути позиція біта(зліва направо) в масиві байт.

namespace meow
{
    enum BtnID : uint8_t
    {
        BTN_PTT = 0,
        BTN_OK,
        BTN_BACK,
        BTN_LEFT,
        BTN_RIGHT,
        BTN_UP,
        BTN_DOWN,
    };
}

// -------------------------------- Макрос з ініціалізацією пінів. Між фігурними дужками потрібно додати ініціалізацію пінів, як показано нижче
// -------------------------------- Де Button(uint8_t key_id, bool is_touch)
#define BUTTONS                                    \
    {                                              \
        {BTN_PTT, new Button(BTN_PTT, false)},     \
        {BTN_OK, new Button(BTN_OK, false)},       \
        {BTN_BACK, new Button(BTN_BACK, false)},   \
        {BTN_LEFT, new Button(BTN_LEFT, false)},   \
        {BTN_RIGHT, new Button(BTN_RIGHT, false)}, \
        {BTN_UP, new Button(BTN_UP, false)},       \
        {BTN_DOWN, new Button(BTN_DOWN, false)},   \
    }

// -------------------------------- Налаштування чутливості сенсорних кнопок
#if defined(CONFIG_IDF_TARGET_ESP32S3)
#define BTN_TOUCH_TRESHOLD 50000
#else
#define BTN_TOUCH_TRESHOLD 65
#endif

#define PRESS_DURATION (unsigned long)1000 // Час, після спливання якого кнопка вважається утримуваною.

#define PRESS_LOCK (unsigned long)700
#define CLICK_LOCK (unsigned long)250
#define HOLD_LOCK (unsigned long)150

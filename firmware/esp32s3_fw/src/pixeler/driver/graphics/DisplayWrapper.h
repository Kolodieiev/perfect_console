#pragma once
#pragma GCC optimize("O3")
#include <stdint.h>
#include <pixeler_setup/display_setup.h>
#include <pixeler_setup/graphics_setup.h>

#include "pixeler/driver/graphics/Arduino_GFX/Arduino_GFX_Library.h"

#define WDT_GUARD_TIME 1000UL

namespace pixeler
{
#ifdef GRAPHICS_ENABLED
  class DisplayWrapper
  {
  public:
    DisplayWrapper();
    ~DisplayWrapper();

    void __flush();

    void fillScreen(uint16_t color);

    void setFont(const uint8_t* font);
    void setTextSize(uint8_t size);
    void setTextColor(uint16_t color);
    void setCursor(int16_t x, int16_t y);
    void setTextWrap(bool state);
    size_t print(const char* str);

    void calcTextBounds(const char* str, int16_t x, int16_t y, int16_t& x_out, int16_t& y_out, uint16_t& w_out, uint16_t& h_out);

    void setRotation(uint8_t rotation);

    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

    void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color);
    void fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color);

    void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);
    void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);

    void drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t radius, uint16_t color);
    void fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t radius, uint16_t color);

    void drawBitmap(int16_t x, int16_t y, const uint16_t* bitmap, int16_t w, int16_t h);
    void drawBitmapTransp(int16_t x, int16_t y, const uint16_t* bitmap, int16_t w, int16_t h);
    void drawBitmapRotated(int16_t x, int16_t y, const uint16_t* bitmap, int16_t w, int16_t h, int16_t piv_x, int16_t piv_y, float angle);

    uint16_t getWidth();
    uint16_t getHeight();

    uint16_t getFontHeight(const uint8_t* font, uint8_t size = 1);

    void init();

#ifdef ENABLE_SCREENSHOTER
    void takeScreenshot();
#endif  // ENABLE_SCREENSHOTER

#ifdef BACKLIGHT_PIN
    /**
     * @brief Вмикає підсвітку дисплея зі 100% яскравістю.
     *
     */
    void enableBackLight();

    /**
     * @brief Вимикає підсвітку дисплея
     *
     */
    void disableBackLight();

#ifdef HAS_BL_PWM
    /**
     * @brief Встановлює яскравість підсвітки дисплея.
     * Де 0 - підсвітка вимкнена, а 255 - рівень яскравості максимальний.
     *
     * @param value
     */
    void setBrightness(uint8_t value);

    /**
     * @brief Повертає значення поточної яскравості підсвітки дисплея.
     *
     * @return uint8_t
     */
    uint8_t getBrightness() const;
#endif  // HAS_BL_PWM
#endif  // BACKLIGHT_PIN

  private:
#ifdef DOUBLE_BUFFERRING
    static void displayRendererTask(void* params);
#endif  // DOUBLE_BUFFERRING

#ifdef ENABLE_SCREENSHOTER
    static void takeScreenshot(DisplayWrapper* self);
#endif  // ENABLE_SCREENSHOTER

  private:
    BUS_TYPE _bus{BUS_PARAMS};
    Arduino_GFX* _output = new DISP_DRIVER_TYPE(&_bus, DISP_DRIVER_PARAMS);
    Arduino_Canvas _canvas{TFT_WIDTH, TFT_HEIGHT, _output, 0, 0, 0};

#ifdef DOUBLE_BUFFERRING
    volatile xSemaphoreHandle _sync_mutex{nullptr};
#endif  // DOUBLE_BUFFERRING

#ifdef SHOW_FPS
    uint64_t _frame_timer{0};
    uint16_t _frame_counter{0};
    uint16_t _temp_frame_counter{0};
#endif  // SHOW_FPS

#ifdef DOUBLE_BUFFERRING
    volatile bool _has_frame{false};
#endif  // DOUBLE_BUFFERRING

    volatile bool _take_screenshot{false};
    bool _is_buff_changed = false;

#ifdef HAS_BL_PWM
    uint8_t _cur_brightness{255};
#endif  // HAS_BL_PWM
  };

  extern DisplayWrapper _display;
#endif  // GRAPHICS_ENABLED
}  // namespace pixeler

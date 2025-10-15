#pragma GCC optimize("O3")
#include "DisplayWrapper.h"

#include <esp32-hal-ledc.h>

#include "pixeler/util/img/BmpUtil.h"

namespace pixeler
{
#ifdef GRAPHICS_ENABLED
  DisplayWrapper::DisplayWrapper()
  {
  }

  DisplayWrapper::~DisplayWrapper()
  {
    log_e("Цього не повинно було статися!");
    esp_restart();
  }

  void DisplayWrapper::fillScreen(uint16_t color)
  {
    _canvas.fillScreen(color);
    _is_buff_changed = true;
  }

  void DisplayWrapper::setCursor(int16_t x, int16_t y)
  {
    _canvas.setCursor(x, y);
  }

  size_t DisplayWrapper::print(const char* str)
  {
    return _canvas.print(str);
    _is_buff_changed = true;
  }

  void DisplayWrapper::setFont(const uint8_t* font)
  {
    _canvas.setFont(font);
  }

  void DisplayWrapper::setTextSize(uint8_t size)
  {
    _canvas.setTextSize(size, size);
  }

  void DisplayWrapper::setTextColor(uint16_t color)
  {
    _canvas.setTextColor(color);
  }

  void DisplayWrapper::calcTextBounds(const char* str, int16_t x, int16_t y, int16_t& x_out, int16_t& y_out, uint16_t& w_out, uint16_t& h_out)
  {
    _canvas.getTextBounds(str, x, y, x_out, y_out, w_out, h_out);
  }

  void DisplayWrapper::setRotation(uint8_t rotation)
  {
    _output->setRotation(rotation);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

  void DisplayWrapper::drawPixel(int16_t x, int16_t y, uint16_t color)
  {
    _canvas.drawPixel(x, y, color);
    _is_buff_changed = true;
  }

  void DisplayWrapper::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
  {
    _canvas.drawLine(x0, y0, x1, y1, color);
    _is_buff_changed = true;
  }

  void DisplayWrapper::drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color)
  {
    _canvas.drawCircle(x, y, r, color);
    _is_buff_changed = true;
  }

  void DisplayWrapper::fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color)
  {
    _canvas.fillCircle(x, y, r, color);
    _is_buff_changed = true;
  }

  void DisplayWrapper::drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
  {
    _canvas.drawRect(x, y, w, h, color);
    _is_buff_changed = true;
  }

  void DisplayWrapper::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
  {
    _canvas.fillRect(x, y, w, h, color);
    _is_buff_changed = true;
  }

  void DisplayWrapper::drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t radius, uint16_t color)
  {
    _canvas.drawRoundRect(x, y, w, h, radius, color);
    _is_buff_changed = true;
  }

  void DisplayWrapper::fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t radius, uint16_t color)
  {
    _canvas.drawRoundRect(x, y, w, h, radius, color);
    _is_buff_changed = true;
  }

  void DisplayWrapper::drawBitmap(int16_t x, int16_t y, const uint16_t* bitmap, int16_t w, int16_t h)
  {
    _canvas.draw16bitRGBBitmap(x, y, bitmap, w, h);
    _is_buff_changed = true;
  }

  void DisplayWrapper::drawBitmapTransp(int16_t x, int16_t y, const uint16_t* bitmap, int16_t w, int16_t h, uint16_t transparent_color)
  {
    _canvas.draw16bitRGBBitmapWithTranColor(x, y, bitmap, transparent_color, w, h);
    _is_buff_changed = true;
  }

  uint16_t DisplayWrapper::getWidth()
  {
    return _canvas.width();
  }

  uint16_t DisplayWrapper::getHeight()
  {
    return _canvas.height();
  }

  uint16_t DisplayWrapper::getFontHeight(const uint8_t* font, uint8_t size)
  {
    int16_t x1, y1;
    uint16_t w, h;
    _display.setTextSize(size);
    _display.setFont(font);
    _display.calcTextBounds("I", 0, 0, x1, y1, w, h);
    return h;
  }

  void DisplayWrapper::init()
  {
    if (BUSS_FREQUENCY < 10000000)
    {
      log_e("Встановлено занадто низьку швидкість шини");
      esp_restart();
    }

    _canvas.begin(BUSS_FREQUENCY);
    _canvas.setUTF8Print(true);
    _canvas.setTextWrap(false);

    setRotation(DISPLAY_ROTATION);

#ifdef DOUBLE_BUFFERRING
    _sync_mutex = xSemaphoreCreateMutex();

    if (!_sync_mutex)
    {
      log_e("Недостатньо ресурсів для роботи графічного драйвера");
      esp_restart();
    }

    BaseType_t result = xTaskCreatePinnedToCore(displayRendererTask, "dRend", 5 * 512, this, 11, nullptr, 0);

    if (result != pdPASS)
    {
      log_e("Недостатньо ресурсів для роботи графічного драйвера");
      esp_restart();
    }
#endif  // DOUBLE_BUFFERRING
  }

#ifdef ENABLE_SCREENSHOTER
  void DisplayWrapper::takeScreenshot()
  {
    _take_screenshot = true;
    _is_buff_changed = true;
  }
#endif  // ENABLE_SCREENSHOTER

#ifdef HAS_BL_PWM
  void DisplayWrapper::enableBackLight()
  {
    pinMode(BACKLIGHT_PIN, OUTPUT);
    digitalWrite(BACKLIGHT_PIN, HIGH);
  }

  void DisplayWrapper::disableBackLight()
  {
    digitalWrite(BACKLIGHT_PIN, LOW);
  }

  void DisplayWrapper::setBrightness(uint8_t value)
  {
    _cur_brightness = value;
    ledcAttach(BACKLIGHT_PIN, PWM_FREQ, PWM_RESOLUTION);
    ledcWrite(BACKLIGHT_PIN, value);
  }
#endif  // BACKLIGHT_PIN

  void DisplayWrapper::__flush()
  {
    if (_is_buff_changed)
    {
      _is_buff_changed = false;

#ifdef SHOW_FPS
      if (millis() - _frame_timer < 1000)
      {
        ++_temp_frame_counter;
      }
      else
      {
        _frame_counter = _temp_frame_counter + 1;
        _temp_frame_counter = 0;
        _frame_timer = millis();
      }

      String fps_str = String(_frame_counter);

      _canvas.setTextSize(1);
      _canvas.setFont(u8g2_font_unifont_t_cyrillic);
      _canvas.setTextColor(COLOR_RED);

      int16_t x{0};
      int16_t y{0};
      int16_t x_out{0};
      int16_t y_out{0};
      uint16_t w{0};
      uint16_t h{0};

      _canvas.getTextBounds(fps_str.c_str(), x, y, x_out, y_out, w, h);
      //
      uint16_t fps_x_pos = _canvas.width() / 2 - w;
      _canvas.fillRect(fps_x_pos - 3, 0, w + 6, h + 9, COLOR_BLACK);
      _canvas.setCursor(fps_x_pos, h + 3);
      _canvas.print(fps_str);
#endif  // SHOW_FPS

#ifdef DOUBLE_BUFFERRING
      xSemaphoreTake(_sync_mutex, portMAX_DELAY);
      _has_frame = true;
      _canvas.duplicateMainBuff();
      xSemaphoreGive(_sync_mutex);
#else
      _canvas.flushMainBuff();

#ifdef ENABLE_SCREENSHOTER
      if (_take_screenshot)
        takeScreenShot(this);
#endif  // ENABLE_SCREENSHOTER

#endif  // DOUBLE_BUFFERRING
    }
  }

#ifdef DOUBLE_BUFFERRING
  void DisplayWrapper::displayRendererTask(void* params)
  {
    DisplayWrapper& self = *static_cast<DisplayWrapper*>(params);

    while (1)
    {
      if (self._has_frame)
      {
        xSemaphoreTake(self._sync_mutex, portMAX_DELAY);

        self._has_frame = false;
        self._canvas.flushSecondBuff();

#ifdef ENABLE_SCREENSHOTER
        if (self._take_screenshot)
          takeScreenShot(&self);
#endif  // ENABLE_SCREENSHOTER
        xSemaphoreGive(self._sync_mutex);
      }
      vTaskDelay(1 / portTICK_PERIOD_MS);
    }
  }
#endif  // DOUBLE_BUFFERRING

#ifdef ENABLE_SCREENSHOTER
  void DisplayWrapper::takeScreenShot(DisplayWrapper* self)
  {
    self->_take_screenshot = false;

    BmpHeader header;
    header.width = self->_canvas.width();
    header.height = self->_canvas.height();

    String path_to_bmp = "/screenshot_";
    path_to_bmp += millis();
    path_to_bmp += ".bmp";

    bool res{false};

#ifdef DOUBLE_BUFFERRING
    res = BmpUtil::saveBmp(header,
                           self->_canvas.getDupFramebuffer(),
                           path_to_bmp.c_str(),
                           true);
#else
    res = BmpUtil::saveBmp(header,
                           self->_canvas.getFramebuffer(),
                           path_to_bmp.c_str(),
                           true);
#endif  // DOUBLE_BUFFERRING

    if (res)
      log_i("Скріншот успішно збережено");
    else
      log_e("Помилка створення скріншоту");
  }
#endif  // ENABLE_SCREENSHOTER

  DisplayWrapper _display;
#endif  // GRAPHICS_ENABLED
}  // namespace pixeler
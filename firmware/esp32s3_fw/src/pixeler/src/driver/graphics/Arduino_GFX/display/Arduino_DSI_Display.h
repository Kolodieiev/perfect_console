#pragma once

#include "../Arduino_DataBus.h"

#if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32P4)
#include <esp_cache.h>

#include "../Arduino_GFX.h"
#include "../databus/Arduino_ESP32DSIPanel.h"
#include "driver/ppa.h"

class Arduino_DSI_Display : public Arduino_GFX
{
public:
  Arduino_DSI_Display(
      Arduino_ESP32DSIPanel* dsi_panel,
      uint16_t w,
      uint16_t h,
      uint8_t rotation = 0,
      int8_t rst = GFX_NOT_DEFINED);

  virtual bool begin(int32_t speed = GFX_NOT_DEFINED) override;
  virtual void draw16bitRGBBitmap(int16_t x, int16_t y, const uint16_t* bitmap, int16_t w, int16_t h) override;
  virtual void writePixelPreclipped(int16_t x, int16_t y, uint16_t color) override;

private:
  static bool IRAM_ATTR lcd_trans_done_cb(esp_lcd_panel_handle_t panel, esp_lcd_dpi_panel_event_data_t* edata, void* user_ctx);

protected:
  ppa_client_handle_t ppa_fill_client{nullptr};
  ppa_client_handle_t ppa_srm_client{nullptr};

  Arduino_ESP32DSIPanel* _dsi_panel;
  esp_lcd_panel_handle_t _panel_handle{nullptr};
  volatile SemaphoreHandle_t _dsi_semaphore{nullptr};

  void* _lcd_draw_buffers[2] = {nullptr, nullptr};
  void* _draw_buffer{nullptr};

  size_t _framebuffer_size{0};

  const uint16_t MAX_X, MAX_Y;
  uint8_t _xStart{0};
  uint8_t _yStart{0};
  uint8_t _back_fb_i{0};
  int8_t _pin_rst;

private:
};

#endif  // #if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32P4)
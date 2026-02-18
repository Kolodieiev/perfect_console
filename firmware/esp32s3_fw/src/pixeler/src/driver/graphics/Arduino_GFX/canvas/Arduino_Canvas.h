#pragma GCC optimize("O3")
#include "../Arduino_DataBus.h"
#if !defined(LITTLE_FOOT_PRINT)

#ifndef _ARDUINO_CANVAS_H_
#define _ARDUINO_CANVAS_H_

#include "../Arduino_GFX.h"

class Arduino_Canvas : public Arduino_GFX
{
public:
  Arduino_Canvas(int16_t w, int16_t h, Arduino_G* output, int16_t output_x = 0, int16_t output_y = 0, uint8_t rotation = 0);
  ~Arduino_Canvas();

  bool begin(int32_t speed = GFX_NOT_DEFINED) override;
  void writePixelPreclipped(int16_t x, int16_t y, uint16_t color) override;
  void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
  void writeFastVLineCore(int16_t x, int16_t y, int16_t h, uint16_t color);
  void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
  void writeFastHLineCore(int16_t x, int16_t y, int16_t w, uint16_t color);
  void writeFillRectPreclipped(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;
  void draw16bitRGBBitmap(int16_t x, int16_t y, const uint16_t* bitmap, int16_t w, int16_t h) override;
  void draw16bitRGBBitmapWithTranColor(int16_t x, int16_t y, const uint16_t* bitmap, uint16_t transparent_color, int16_t w, int16_t h) override;  // TODO const bitmap
  void flushMainBuff() override;
  virtual void duplicateMainBuff() override;
  virtual void flushSecondBuff() override;

  uint16_t* getFramebuffer();
  uint16_t* getDupFramebuffer();

protected:
  uint16_t* _framebuffer{nullptr};
  uint16_t* _framebuffer2{nullptr};
  Arduino_G* _output{nullptr};
  size_t _buff_size{0};
  int16_t _output_x{0};
  int16_t _output_y{0};
  const uint16_t MAX_X{0};
  const uint16_t MAX_Y{0};

private:
};

#endif  // _ARDUINO_CANVAS_H_

#endif  // !defined(LITTLE_FOOT_PRINT)

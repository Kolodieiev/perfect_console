#pragma GCC optimize("O3")
#ifndef _ARDUINO_GFX_LIBRARIES_H_
#define _ARDUINO_GFX_LIBRARIES_H_

#define U8G2_FONT_SUPPORT

#include <pixeler_setup/display_setup.h>
#include <pixeler_setup/graphics_setup.h>

#include "Arduino_DataBus.h"
#include "Arduino_GFX.h"  // Core graphics library
#include "databus/Arduino_ESP32DSIPanel.h"
#include "databus/Arduino_ESP32PAR16.h"
#include "databus/Arduino_ESP32PAR16Q.h"
#include "databus/Arduino_ESP32PAR16QQ.h"
#include "databus/Arduino_ESP32PAR8.h"
#include "databus/Arduino_ESP32PAR8Q.h"
#include "databus/Arduino_ESP32PAR8QQ.h"
#include "databus/Arduino_ESP32PAR8QQQ.h"
#include "databus/Arduino_ESP32QSPI.h"
#include "databus/Arduino_ESP32RGBPanel.h"
#include "databus/Arduino_ESP32S2PAR16.h"
#include "databus/Arduino_ESP32S2PAR16Q.h"
#include "databus/Arduino_ESP32S2PAR8.h"
#include "databus/Arduino_ESP32S2PAR8Q.h"
#include "databus/Arduino_ESP32SPI.h"
#if !defined(LITTLE_FOOT_PRINT)
#include "canvas/Arduino_Canvas.h"
#endif  // !defined(LITTLE_FOOT_PRINT)

#include "display/Arduino_AXS15231B.h"
#include "display/Arduino_DSI_Display.h"
#include "display/Arduino_GC9106.h"
#include "display/Arduino_GC9107.h"
#include "display/Arduino_GC9A01.h"
#include "display/Arduino_GC9C01.h"
#include "display/Arduino_GC9D01.h"
#include "display/Arduino_HX8347C.h"
#include "display/Arduino_HX8347D.h"
#include "display/Arduino_HX8352C.h"
#include "display/Arduino_HX8357A.h"
#include "display/Arduino_HX8357B.h"
#include "display/Arduino_HX8369A.h"
#include "display/Arduino_ILI9225.h"
#include "display/Arduino_ILI9331.h"
#include "display/Arduino_ILI9341.h"
#include "display/Arduino_ILI9342.h"
#include "display/Arduino_ILI9481_18bit.h"
#include "display/Arduino_ILI9486.h"
#include "display/Arduino_ILI9486_18bit.h"
#include "display/Arduino_ILI9488.h"
#include "display/Arduino_ILI9488_18bit.h"
#include "display/Arduino_ILI9806.h"
#include "display/Arduino_JBT6K71.h"
#include "display/Arduino_NT35310.h"
#include "display/Arduino_NT35510.h"
#include "display/Arduino_NT39125.h"
#include "display/Arduino_NV3007.h"
#include "display/Arduino_NV3023.h"
#include "display/Arduino_NV3041A.h"
#include "display/Arduino_OTM8009A.h"
#include "display/Arduino_R61529.h"
#include "display/Arduino_RGB_Display.h"
#include "display/Arduino_RM67162.h"
#include "display/Arduino_SEPS525.h"
#include "display/Arduino_SPD2010.h"
#include "display/Arduino_ST7735.h"
#include "display/Arduino_ST7789.h"
#include "display/Arduino_ST77916.h"
#include "display/Arduino_ST7796.h"
#include "display/Arduino_WEA2012.h"

// Arduino_DataBus *create_default_Arduino_DataBus();
// Arduino_GFX *create_default_Arduino_GFX();

#endif  // _ARDUINO_GFX_LIBRARIES_H_

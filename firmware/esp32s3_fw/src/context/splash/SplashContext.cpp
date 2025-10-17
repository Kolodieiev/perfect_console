#include "SplashContext.h"

#include "../WidgetCreator.h"
#include "pixeler/manager/I2C_Manager.h"
#include "pixeler/manager/SD_Manager.h"
#include "pixeler/manager/SPI_Manager.h"
#include "pixeler/manager/SettingsManager.h"
#include "pixeler_setup/sd_setup.h"

#define OFFSET_LBL_INFO 0
#define OFFSET_LBL_RESULT 70
#define SHOW_INIT_TIME 2000

SplashContext::SplashContext()
{
  _display.enableBackLight();
  _start_time = millis();

  WidgetCreator creator;
  EmptyLayout* layout = creator.getEmptyLayout();
  setLayout(layout);
  //
  uint16_t y_pos{0};

  SPI_Manager::initBus(SD_SPI_BUS, SD_PIN_SCLK, SD_PIN_MISO, SD_PIN_MOSI);
  _sd.mount(pixeler::SPI_Manager::getSpi4Bus(SD_SPI_BUS));

  // SDCARD
  if (_sd.isMounted())
  {
    addLabel(OFFSET_LBL_INFO, y_pos, STR_SUCCESS, COLOR_GREEN);

    String bright = SettingsManager::get(STR_PREF_BRIGHT);

    if (bright.equals("") || bright.equals("0"))
      _display.setBrightness(240);
    else
      _display.setBrightness(atoi(bright.c_str()));
  }
  else
  {
    addLabel(OFFSET_LBL_INFO, y_pos, STR_FAIL, COLOR_RED);
  }

  addLabel(OFFSET_LBL_RESULT, y_pos, "Ініціалізація SD", COLOR_WHITE);

  y_pos += _display.getFontHeight(font_unifont) + 3;

  // PSRAM
  bool psram_ok = psramInit();
  if (psram_ok)
    addLabel(OFFSET_LBL_INFO, y_pos, STR_SUCCESS, COLOR_GREEN);
  else
    addLabel(OFFSET_LBL_INFO, y_pos, STR_FAIL, COLOR_RED);

  addLabel(OFFSET_LBL_RESULT, y_pos, "Ініціалізація PSRAM", COLOR_WHITE);
}

bool SplashContext::loop()
{
  return true;
}

void SplashContext::update()
{
  if (millis() - _start_time > SHOW_INIT_TIME)
  {
    openContextByID(ContextID::ID_CONTEXT_HOME);
  }
}

void SplashContext::addLabel(uint16_t x_pos, uint16_t y_pos, const char* str, uint16_t color)
{
  EmptyLayout* layout = getLayout()->castTo<EmptyLayout>();

  Label* lbl = new Label(_widget_id);
  layout->addWidget(lbl);
  lbl->setText(str);
  lbl->setBackColor(COLOR_MAIN_BACK);
  lbl->setTextColor(color);
  lbl->setPos(x_pos, y_pos);
  lbl->initWidthToFit();

  ++_widget_id;
}

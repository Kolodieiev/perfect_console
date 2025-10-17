#include "HomeContext.h"

#include "../WidgetCreator.h"
#include "../resources/ico/battery.h"
#include "pixeler/manager/SettingsManager.h"
#include "pixeler/util/batt_util.h"
#include "pixeler/util/img/BmpUtil.h"

#define UPD_DISPLAY_INTERVAL_MS 5000UL

HomeContext::HomeContext()
{
  WidgetCreator creator;

  EmptyLayout* layout = creator.getEmptyLayout();
  setLayout(layout);

  Image* wallpp_img = new Image(ID_WALLPAPER);

  String walpp_path = SettingsManager::get(STR_WALLPP_FILENAME);

  if (walpp_path.isEmpty())
    walpp_path = "/wall/wallpaper.bmp";

  ImgData bmp = BmpUtil::loadBmp(walpp_path.c_str());

  if (bmp.data_ptr)
  {
    _wallpaper_ptr = bmp.data_ptr;
    wallpp_img->setWidth(bmp.width);
    wallpp_img->setHeight(bmp.height);
    wallpp_img->setSrc(_wallpaper_ptr);
    layout->addWidget(wallpp_img);
  }

  _batt_volt_lbl = new Label(ID_BAT_LVL);
  layout->addWidget(_batt_volt_lbl);
  _batt_volt_lbl->setText(STR_EMPTY_BAT);
  _batt_volt_lbl->setWidth(ICO_BATTERY_WIDTH);
  _batt_volt_lbl->setHeight(ICO_BATTERY_HEIGHT);
  _batt_volt_lbl->setFont(font_5x7);
  _batt_volt_lbl->setAlign(Label::ALIGN_CENTER);
  _batt_volt_lbl->setGravity(Label::GRAVITY_CENTER);
  _batt_volt_lbl->setPos(TFT_WIDTH - DISPLAY_PADDING - _batt_volt_lbl->getWidth(), DISPLAY_PADDING);
  _batt_volt_lbl->setTransparency(true);

  _batt_ico = new Image(1);
  _batt_volt_lbl->setBackImg(_batt_ico);
  _batt_ico->setWidth(_batt_volt_lbl->getWidth());
  _batt_ico->setHeight(_batt_volt_lbl->getHeight());
  _batt_ico->setSrc(ICO_BATTERY);
  _batt_ico->setTransparency(true);

  updateBattVoltage();
}

HomeContext::~HomeContext()
{
  delete _batt_ico;
  free(_wallpaper_ptr);
}

bool HomeContext::loop()
{
  return true;
}

void HomeContext::update()
{
  if (_input.isReleased(BtnID::BTN_OK))
  {
    _input.lock(BtnID::BTN_OK, CLICK_LOCK);
    openContextByID(ID_CONTEXT_MENU);
  }

  if (millis() - _upd_timer > UPD_DISPLAY_INTERVAL_MS)
  {
    _upd_timer = millis();
    updateBattVoltage();
  }
}

void HomeContext::updateBattVoltage()
{
  float bat_voltage = readBattVoltage();
  String volt_str = String(bat_voltage);
  _batt_volt_lbl->setText(volt_str);

  getLayout()->forcedDraw();
}

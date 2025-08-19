#include "HomeContext.h"
#include "../WidgetCreator.h"
#include "meow/util/img/BmpUtil.h"
#include "../resources/ico/battery.h"
#include "meow/util/battery/BatteryUtil.h"

#define UPD_DISPLAY_INTERVAL_MS 5000UL

const char EMPTY_BAT[] = "0.00";

HomeContext::HomeContext()
{
    WidgetCreator creator;

    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    Image *wallpp_img = new Image(ID_WALLPAPER);

    BmpUtil util;
    ImgData bmp = util.loadBmp("/wall/wallpaper.bmp");

    if (bmp.data_ptr)
    {
        _wallpaper_ptr = bmp.data_ptr;
        wallpp_img->init(bmp.width, bmp.height);
        wallpp_img->setSrc(_wallpaper_ptr);
        layout->addWidget(wallpp_img);
    }

    _bat_volt_lbl = new Label(ID_BAT_LVL);
    layout->addWidget(_bat_volt_lbl);
    _bat_volt_lbl->setText(EMPTY_BAT);
    _bat_volt_lbl->setWidth(56);
    _bat_volt_lbl->setHeight(35);
    _bat_volt_lbl->setAlign(Label::ALIGN_CENTER);
    _bat_volt_lbl->setGravity(Label::GRAVITY_CENTER);
    _bat_volt_lbl->setPos(_display.width() - TFT_CUTOUT - DISPLAY_PADDING - _bat_volt_lbl->getWidth(), DISPLAY_PADDING);
    _bat_volt_lbl->setTransparency(true);

    _bat_ico = new Image(1);
    _bat_volt_lbl->setBackImg(_bat_ico);
    _bat_ico->init(_bat_volt_lbl->getWidth(), _bat_volt_lbl->getHeight());
    _bat_ico->setSrc(ICO_BATTERY);
    _bat_ico->setTransparency(true);
    _bat_ico->setTranspColor(TFT_TRANSPARENT);

    updateBatVoltage();
}

HomeContext::~HomeContext()
{
    delete _bat_ico;
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
        updateBatVoltage();
    }
}

void HomeContext::updateBatVoltage()
{
    float bat_voltage = BatteryUtil::readVoltVal();
    String volt_str = String(bat_voltage);
    _bat_volt_lbl->setText(volt_str);

    getLayout()->forcedDraw();
}

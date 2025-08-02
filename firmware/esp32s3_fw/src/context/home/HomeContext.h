#pragma once
#include <Arduino.h>
#include "meow/ui/context/IContext.h"
#include "meow/ui/widget/text/Label.h"

using namespace meow;

class HomeContext : public IContext
{
public:
    HomeContext();
    virtual ~HomeContext();

protected:
    virtual bool loop() override;
    virtual void update() override;
    //
private:
    enum Widget_ID : uint8_t
    {
        ID_WALLPAPER = 1,
        ID_BAT_LVL,
        ID_TIME_LBL,
        ID_DATE_LBL,
        ID_DAY_LBL,
    };

    Image *_bat_ico;
    Label *_bat_cap_lbl{nullptr};
    unsigned long _upd_timer{0};
    uint16_t *_wallpaper_ptr{nullptr};

    void updateBatCap();
};

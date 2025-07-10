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
    };
};

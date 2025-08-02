#pragma once
#include <Arduino.h>
#include "meow/ui/context/IContext.h"

using namespace meow;

class SplashContext : public IContext
{
public:
    SplashContext();
    virtual ~SplashContext() {};

protected:
    virtual bool loop() override;
    virtual void update() override;
    //
private:
    void addLabel(uint16_t x_pos, uint16_t y_pos, const char *str, uint16_t color);

private:
    unsigned long _start_time;
    uint8_t _widget_id{1};
};
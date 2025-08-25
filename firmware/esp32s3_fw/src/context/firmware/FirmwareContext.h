#pragma once
#include <Arduino.h>
#include "meow/ui/context/IContext.h"

using namespace meow;

class FirmwareContext : public IContext
{
public:
    FirmwareContext();
    virtual ~FirmwareContext(){}

protected:
    virtual bool loop() override;
    virtual void update() override;

private:
    enum Widget_ID : uint8_t
    {
        ID_NAVBAR = 1,
        ID_TITLE,
        ID_AUTHOR,
        ID_VERSION,
    };

    void showUpdating();
};
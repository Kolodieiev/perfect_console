#pragma once
#include <Arduino.h>

#include "meow/ui/context/IContext.h"
#include "meow/ui/widget/progress/ProgressBar.h"

using namespace meow;

class PrefBrightContext : public IContext
{
public:
    PrefBrightContext();
    virtual ~PrefBrightContext() {}

protected:
    virtual bool loop() override;
    virtual void update() override;

private:
    enum Widget_ID : uint8_t
    {
        ID_NAVBAR = 2,
        ID_HEADER,
        ID_PROGRESSBAR
    };

    uint8_t _old_bright;
    ProgressBar *_progress;
};
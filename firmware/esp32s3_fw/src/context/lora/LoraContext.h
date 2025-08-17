#pragma once
#include <Arduino.h>

#include "meow/ui/context/IContext.h"

namespace meow
{
    class LoraContext : public IContext
    {
    public:
        LoraContext();
        virtual ~LoraContext() {}

    protected:
        virtual bool loop() override;
        virtual void update() override;

    private:
        enum Mode : uint8_t
        {
            MODE_MAIN = 0,
            MODE_SETUP,
            MODE_SD_UNCONN,
        };

        enum Widget_ID : uint8_t
        {
            ID_MENU = 1,
        };

        void showMainTmpl();
        void showSDErrTmpl();

    private:
        Mode _mode{MODE_MAIN};
    };
}
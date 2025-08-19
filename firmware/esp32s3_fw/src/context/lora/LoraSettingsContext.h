#pragma once
#include <Arduino.h>
#include "meow/ui/context/IContext.h"

const char STR_GENERATE_KEY[] = "Згенерувати ключ";

namespace meow
{
    class LoraSettingsContext : public IContext
    {
    public:
        LoraSettingsContext();
        virtual ~LoraSettingsContext() {}

    protected:
        virtual bool loop() override;
        virtual void update() override;

    private:
        enum Widget_ID : uint8_t
        {
            ID_MENU = 1,
        };

        void showMainTmpl();
        void showSDErrTmpl();
    };
}
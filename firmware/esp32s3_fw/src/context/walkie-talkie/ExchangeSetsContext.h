#pragma once
#include <Arduino.h>
#include "meow/ui/context/IContext.h"

using namespace meow;

class ExchangeSetsContext : public IContext
{
public:
    ExchangeSetsContext();
    virtual ~ExchangeSetsContext();

protected:
    virtual bool loop() override;
    virtual void update() override;

private:
    enum Mode : uint8_t
    {
        MODE_MAIN = 0,
        MODE_CTX_MENU,
    };

    enum Widget_ID : uint8_t
    {
        ID_MAIN_MENU = 1,
        ID_CONTEXT_MENU,
        //
    };

    void showMainTmpl();
    void showContextMenuTmpl();
    //

private:
};

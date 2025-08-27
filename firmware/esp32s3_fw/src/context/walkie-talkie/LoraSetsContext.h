#pragma once
#include <Arduino.h>
#include "meow/ui/context/IContext.h"
#include "LoraSettingsStruct.h"

using namespace meow;

class LoraSetsContext : public IContext
{
public:
    LoraSetsContext();
    virtual ~LoraSetsContext();

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
        ID_SAVE_SETS_ITEM,
    };

    void showMainTmpl();
    void showContextMenuTmpl();
    //
    void loadLoraSettings();

private:
    LoraSettings _lora_sets;

    Mode _mode{MODE_MAIN};
};

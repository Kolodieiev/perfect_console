#pragma once
#include <Arduino.h>
#include "meow/ui/context/IContext.h"

#include "meow/ui/widget/text/Label.h"
#include "meow/ui/widget/menu/FixedMenu.h"
#include "meow/lib/lora/LRE32.h"
#include "meow/lib/lora/LRE220T.h"
#include "LoraSettingsStruct.h"

namespace meow
{
    class LoraContext : public IContext
    {
    public:
        LoraContext();
        virtual ~LoraContext();

    protected:
        virtual bool loop() override;
        virtual void update() override;

    private:
        enum Mode : uint8_t
        {
            MODE_MAIN = 0,
            MODE_SETUP,
            MODE_SD_UNCONN,
            MODE_LORA_UNCONN,
        };

        enum Widget_ID : uint8_t
        {
            ID_CTX_MENU = 1,
            ID_CHNL_TITLE,
            ID_CHNL_VAL,
            ID_ENCRYPT_TITLE,
            ID_ENCRYPT_VAL,
            ID_POWER_TITLE,
            ID_POWER_VAL,
            ID_BAT_VAL,
            ID_LBL_STATE,
        };

        void showSDErrTmpl();
        void showLoraErrTmpl();
        void showMainTmpl();
        void loadLoraSettings();
        void updateBattVoltage();

    private:
        LRE32 _lora; // Зміни тип на LRE220T якщо модуль лори -  LRE220.

        LoraSettings _lora_sets;

        Image *_batt_ico{nullptr};
        Label *_state_val_lbl{nullptr};
        Label *_encrypt_val_lbl{nullptr};
        Label *_power_val_lbl{nullptr};
        Label *_batt_val_lbl{nullptr};
        FixedMenu *_context_menu{nullptr};

        unsigned long _upd_batt_ts{0};

        Mode _mode{MODE_MAIN};

        bool _is_encoding_en{false};
    };
}
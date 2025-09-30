#pragma once
#include <Arduino.h>
#include "meow/ui/context/IContext.h"
#include "meow/ui/widget/text/Label.h"
#include "meow/ui/widget/menu/FixedMenu.h"
#include "meow/ui/widget/scrollbar/ScrollBar.h"
#include "meow/ui/widget/keyboard/Keyboard.h"
#include "meow/ui/widget/text/TextBox.h"
#include "LoraSettingsStruct.h"

using namespace meow;

class SetsEditorContext : public IContext
{
public:
    explicit SetsEditorContext(const String &sets_filename);
    virtual ~SetsEditorContext();

protected:
    virtual bool loop() override;
    virtual void update() override;

private:
    enum Mode : uint8_t
    {
        MODE_MAIN = 0,
        MODE_ENTER_NAME,
        MODE_SEL_POWER,
        MODE_SEL_CHANN,
    };

    enum Widget_ID : uint8_t
    {
        ID_HEADER_LBL = 1,
        ID_MAIN_MENU,
        ID_MAIN_SCROLL,
        ID_DIALOG_TXT,
        ID_KEYBOARD,
        ID_ITEM_ENC,
        ID_ITEM_CHANN,
        ID_ITEM_POWER,
        ID_ITEM_GEN_KEY,
    };

    void showMainTmpl();
    void showNameDialogTmpl();

    void clickOk();
    void clickUp();
    void clickDown();
    void clickLeft();
    void clickRight();
    void clickBack();
    void backPressed();

    void saveSets();

private:
    LoraSettings _lora_sets;

    String _filename;

    FixedMenu *_main_menu{nullptr};
    ScrollBar *_main_scrollbar{nullptr};
    TextBox *_dialog_txt;
    Keyboard *_keyboard;

    Mode _mode{MODE_MAIN};

    bool _is_new{true};
    bool _is_item_locked{false};
};

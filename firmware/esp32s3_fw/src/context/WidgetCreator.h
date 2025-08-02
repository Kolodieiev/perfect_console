/*
 * Генератор віджетів.
 * Використовується тут для часткового очищення файлів вікон від однотипного коду.
 */

#pragma once
#include <Arduino.h>
#include "meow/driver/graphics/GraphicsDriver.h"
#include "resources/strings.h"
#include "resources/colors.h"
#include "resources/kb_btn_id.h"
#include "resources/ch32_pins_def.h"
#include "resources/ui_const.h"

#include "meow/ui/widget/layout/EmptyLayout.h"
#include "meow/ui/widget/text/Label.h"
#include "meow/ui/widget/image/Image.h"
#include "meow/ui/widget/menu/item/MenuItem.h"
#include "meow/ui/widget/menu/DynamicMenu.h"
#include "meow/ui/widget/keyboard/Keyboard.h"

using namespace meow;

class WidgetCreator
{
public:
    EmptyLayout *getEmptyLayout();
    Label *getItemLabel(const char *text, uint8_t font_id = 2, uint8_t text_size = 1);
    MenuItem *getMenuItem(uint16_t id = 1);
    DynamicMenu *getDynamicMenu(uint16_t id);
    Label *getStatusMsgLable(uint16_t id, const char *text, uint8_t text_size = 1);
    Keyboard *getStandardEnKeyboard(uint16_t id);
    Keyboard *getCapsdEnKeyboard(uint16_t id);
};

#include "LoraSetsContext.h"
#include "meow/manager/SettingsManager.h"
#include "../WidgetCreator.h"

const char STR_GENERATE_KEY[] = "Згенерувати ключ";

LoraSetsContext::LoraSetsContext()
{
    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    SettingsManager::load(&_lora_sets, sizeof(LoraSettings), STR_LORA_SETS_NAME, STR_LORA_SETS_DIR);
    showMainTmpl();
}

LoraSetsContext::~LoraSetsContext()
{
}

bool LoraSetsContext::loop()
{
    return true;
}

void LoraSetsContext::update()
{
    if (_input.isPressed(BtnID::BTN_BACK))
    {
        _input.lock(BtnID::BTN_BACK, PRESS_LOCK);
        release();
    }
}

void LoraSetsContext::showMainTmpl()
{
    // TODO Відображати список файлів налаштувань.
}

void LoraSetsContext::showContextMenuTmpl()
{
    // TODO Додати в контексте меню пункт видалити/створити налаштування.
}

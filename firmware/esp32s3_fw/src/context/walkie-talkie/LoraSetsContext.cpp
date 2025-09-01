#include "LoraSetsContext.h"
#include "meow/manager/settings/SettingsManager.h"
#include "../WidgetCreator.h"

#include "meow/lib/qr/QR_Gen.h" // TODO Обмін налаштуваннями винести в окремий контекст

const char STR_GENERATE_KEY[] = "Згенерувати ключ";

LoraSetsContext::LoraSetsContext()
{
    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    loadLoraSettings();
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

void LoraSetsContext::loadLoraSettings() // TODO Додати в налаштування ім'я поточних налаштувань. Завантажувати по імені файлу.
{
    String sets_path = SettingsManager::getSettingsFilePath(""); //TODO
    sets_path += STR_LORA_SETS_DIR;

    if (!_fs.dirExist(sets_path.c_str(), true))
        return;

    sets_path += STR_LORA_SETS_NAME;

    if (_fs.fileExist(sets_path.c_str()))
        _fs.readFile(sets_path.c_str(), &_lora_sets, sizeof(_lora_sets));
}
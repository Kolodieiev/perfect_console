#include "SetsEditorContext.h"
#include "../WidgetCreator.h"

// TODO
// Діалог вводу назви налаштування під час збереження
//
// Головне меню з наступними пунктами
// Шифрування за промовчанням
// Вибір каналу
// Вибір потужності
// Генерація ключа showNotification(_confirm_notification); gen key

const char STR_EDIT_SETS[] = "Редагувати налаштування";
const char STR_NEW_SETS[] = "Нове налаштуваннями";
const char STR_GENERATE_KEY[] = "Згенерувати ключ";
const char *POWER_VALUES[4] = {STR_POWER_MAX_VAL, STR_POWER_HIGH_VAL, STR_POWER_MID_VAL, STR_POWER_MIN_VAL};

SetsEditorContext::SetsEditorContext(const String &sets_file_name) : _file_name{sets_file_name}
{
    _is_new = _file_name.isEmpty() ? true : false;

    // TODO create common notification widget

    // SettingsManager::load(&_lora_sets, sizeof(LoraSettings), STR_LORA_SETS_NAME, STR_LORA_SETS_DIR);
}

SetsEditorContext::~SetsEditorContext()
{
    delete _confirm_notification;
}

bool SetsEditorContext::loop()
{
    return true;
}

void SetsEditorContext::update()
{
    if (_input.isHolded(BtnID::BTN_UP))
    {
        _input.lock(BtnID::BTN_UP, HOLD_LOCK);
        clickUp();
    }
    else if (_input.isHolded(BtnID::BTN_DOWN))
    {
        _input.lock(BtnID::BTN_DOWN, HOLD_LOCK);
        clickDown();
    }
    else if (_input.isReleased(BtnID::BTN_OK))
    {
        _input.lock(BtnID::BTN_OK, CLICK_LOCK);
        clickOk();
    }
    else if (_input.isReleased(BtnID::BTN_BACK))
    {
        _input.lock(BtnID::BTN_BACK, CLICK_LOCK);
        clickBack();
    }
    else if (_input.isPressed(BtnID::BTN_OK))
    {
        _input.lock(BtnID::BTN_OK, PRESS_LOCK);
        saveSets();
    }
    else if (_input.isPressed(BtnID::BTN_BACK))
    {
        _input.lock(BtnID::BTN_BACK, PRESS_LOCK);
        release();
    }
}

void SetsEditorContext::showMainTmpl()
{
    // TODO set header text
    Label *context_header{nullptr};
}

void SetsEditorContext::saveSets()
{
    // TODO
}

void SetsEditorContext::showNameDialog()
{
    // TODO
}

void SetsEditorContext::clickOk()
{
    // TODO
}

void SetsEditorContext::clickBack()
{
    // TODO
}

void SetsEditorContext::clickUp()
{
    // TODO
}

void SetsEditorContext::clickDown()
{
    // TODO
}

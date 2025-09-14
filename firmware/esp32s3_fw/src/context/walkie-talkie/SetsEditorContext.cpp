#include "SetsEditorContext.h"
#include "../WidgetCreator.h"
#include "meow/manager/SettingsManager.h"
#include "meow/ui/widget/menu/item/ToggleItem.h"
#include "meow/ui/widget/menu/item/SpinItem.h"
#include "meow/ui/widget/menu/item/ComboItem.h"
#include "meow/util/aes256.h"

#define MAX_LORA_CHANN 32u // Максимальна кількість каналів, що підтримуються поточним модулем LoRa.
#define MENU_ITEMS_ON_SCREEN 5U

const char STR_NAME_SETS[] = "Ім'я налаштування";
const char STR_EDIT_SETS[] = "Редагувати налаштування";
const char STR_NEW_SETS[] = "Нове налаштування";
const char STR_GENERATE_KEY[] = "Згенерувати ключ";

const char STR_COMBO_MIN_VAL[] = "Потужність: Мінімальна";
const char STR_COMBO_MID_VAL[] = "Потужність: Середня";
const char STR_COMBO_HIGH_VAL[] = "Потужність: Висока";
const char STR_COMBO_MAX_VAL[] = "Потужність: Максимальна";

const char *POWER_VALUES[4] = {STR_COMBO_MAX_VAL, STR_COMBO_HIGH_VAL, STR_COMBO_MID_VAL, STR_COMBO_MIN_VAL};

SetsEditorContext::SetsEditorContext(const String &sets_file_name) : _file_name{sets_file_name}
{
    if (!_file_name.isEmpty())
    {
        _is_new = false;
        _file_name += STR_LORA_SETS_EXT;
        SettingsManager::load(&_lora_sets, sizeof(LoraSettings), _file_name.c_str(), STR_LORA_SETS_DIR);
    }

    showMainTmpl();
}

SetsEditorContext::~SetsEditorContext()
{
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
    else if (_input.isHolded(BtnID::BTN_LEFT))
    {
        _input.lock(BtnID::BTN_LEFT, HOLD_LOCK);
        clickLeft();
    }
    else if (_input.isHolded(BtnID::BTN_RIGHT))
    {
        _input.lock(BtnID::BTN_RIGHT, HOLD_LOCK);
        clickRight();
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
        backPressed();
    }
}

void SetsEditorContext::showMainTmpl()
{
    _mode = MODE_MAIN;

    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    Label *ctx_header_lbl = new Label(ID_HEADER_LBL);
    layout->addWidget(ctx_header_lbl);
    ctx_header_lbl->setWidth(D_WIDTH);
    ctx_header_lbl->setHeight(18);
    ctx_header_lbl->setBackColor(TFT_DARKGREY);
    ctx_header_lbl->setAlign(IWidget::ALIGN_CENTER);
    ctx_header_lbl->setGravity(IWidget::GRAVITY_CENTER);
    ctx_header_lbl->setTextColor(TFT_RED);

    if (_is_new)
        ctx_header_lbl->setText(STR_NEW_SETS);
    else
        ctx_header_lbl->setText(STR_EDIT_SETS);

    _main_menu = new FixedMenu(ID_MAIN_MENU);
    layout->addWidget(_main_menu);
    _main_menu->setBackColor(COLOR_MAIN_BACK);
    _main_menu->setWidth(_display.width() - SCROLLBAR_WIDTH - DISPLAY_PADDING * 2);
    _main_menu->setHeight(_display.height() - TFT_CUTOUT * 2 - ctx_header_lbl->getHeight());
    _main_menu->setItemHeight((_main_menu->getHeight() - 2) / MENU_ITEMS_ON_SCREEN);
    _main_menu->setPos(DISPLAY_PADDING, ctx_header_lbl->getYPos() + ctx_header_lbl->getHeight());
    //
    _main_scrollbar = new ScrollBar(ID_MAIN_SCROLL);
    layout->addWidget(_main_scrollbar);
    _main_scrollbar->setWidth(SCROLLBAR_WIDTH);
    _main_scrollbar->setHeight(_main_menu->getHeight() - 4);
    _main_scrollbar->setPos(_main_menu->getWidth() + _main_menu->getXPos(), _main_menu->getYPos() + 2);
    _main_scrollbar->setBackColor(COLOR_MAIN_BACK);

    // Шифрування
    ToggleItem *enc_toggle_item = new ToggleItem(ID_ITEM_ENC);
    _main_menu->addItem(enc_toggle_item);
    enc_toggle_item->setFocusBorderColor(TFT_LIME);
    enc_toggle_item->setFocusBackColor(COLOR_FOCUS_BACK);
    enc_toggle_item->setChangingBorder(true);
    enc_toggle_item->setChangingBack(true);

    Label *enc_toggle_lbl = creator.getItemLabel(STR_ENCRYPT_TITLE, 4, 2);
    enc_toggle_item->setLbl(enc_toggle_lbl);

    ToggleSwitch *toggle_agc_in = new ToggleSwitch(1);
    enc_toggle_item->setToggle(toggle_agc_in);
    toggle_agc_in->setWidth(40);
    toggle_agc_in->setHeight(20);
    toggle_agc_in->setCornerRadius(7);
    toggle_agc_in->setOn(_lora_sets.encrypt_en);

    // Вибір каналу
    SpinItem *chann_spin_item = new SpinItem(ID_ITEM_CHANN);
    _main_menu->addItem(chann_spin_item);
    chann_spin_item->setFocusBorderColor(TFT_LIME);
    chann_spin_item->setFocusBackColor(COLOR_FOCUS_BACK);
    chann_spin_item->setChangingBorder(true);
    chann_spin_item->setChangingBack(true);

    Label *chann_spin_lbl = creator.getItemLabel(STR_CHANN_TITLE, 4, 2);
    chann_spin_item->setLbl(chann_spin_lbl);

    SpinBox *chann_spin = chann_spin_item->getSpin();
    chann_spin->setMax(MAX_LORA_CHANN);
    chann_spin->setMin(0);
    chann_spin->setStep(1);
    chann_spin->setValue(_lora_sets.channel);
    chann_spin->setWidth(60);
    chann_spin->setTextSize(2);
    chann_spin->setBackColor(TFT_WHITE);
    chann_spin->setTextColor(TFT_BLACK);
    chann_spin->setCornerRadius(5);

    // Вибір потужності
    ComboItem *power_item = new ComboItem(ID_ITEM_POWER);
    _main_menu->addItem(power_item);
    power_item->setFocusBorderColor(TFT_LIME);
    power_item->setFocusBackColor(COLOR_FOCUS_BACK);
    power_item->setChangingBorder(true);
    power_item->setChangingBack(true);
    Label *power_lbl = creator.getItemLabel(emptyString.c_str(), 4, 2);
    power_item->setLbl(power_lbl);

    std::vector<String> vec(POWER_VALUES, POWER_VALUES + 4);
    power_item->setRange(vec);
    power_item->selectPos(_lora_sets.power);

    // Генерація ключа
    MenuItem *gen_key_item = power_item->clone(ID_ITEM_GEN_KEY);
    _main_menu->addItem(gen_key_item);
    gen_key_item->getLbl()->setText(STR_GENERATE_KEY);

    _main_scrollbar->setMax(_main_menu->getSize());
}

void SetsEditorContext::saveSets()
{
    if (_mode == MODE_ENTER_NAME)
    {
        _file_name = _dialog_txt->getText();

        if (_file_name.isEmpty())
        {
            showToast(STR_FAIL);
            return;
        }

        if (_is_new)
            _file_name += STR_LORA_SETS_EXT;
    }
    else if (_is_new)
    {
        showNameDialogTmpl();
        return;
    }

    const char *str_ptr = STR_FAIL;
    if (SettingsManager::save(&_lora_sets, sizeof(LoraSettings), _file_name.c_str(), STR_LORA_SETS_DIR))
    {
        str_ptr = STR_SUCCSESS;
        _is_new = false;
    }
    showToast(str_ptr);

    showMainTmpl();
}

void SetsEditorContext::showNameDialogTmpl()
{
    _mode = MODE_ENTER_NAME;

    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);
    layout->setBackColor(TFT_BLACK);

    Label *ctx_header_lbl = new Label(ID_HEADER_LBL);
    layout->addWidget(ctx_header_lbl);
    ctx_header_lbl->setWidth(D_WIDTH);
    ctx_header_lbl->setHeight(18);
    ctx_header_lbl->setBackColor(TFT_DARKGREY);
    ctx_header_lbl->setAlign(IWidget::ALIGN_CENTER);
    ctx_header_lbl->setGravity(IWidget::GRAVITY_CENTER);
    ctx_header_lbl->setTextColor(TFT_RED);
    ctx_header_lbl->setText(STR_NAME_SETS);

    _dialog_txt = new TextBox(ID_DIALOG_TXT);
    layout->addWidget(_dialog_txt);
    _dialog_txt->setTextOffset(5);
    _dialog_txt->setWidth(D_WIDTH - 10);
    _dialog_txt->setHeight(40);
    _dialog_txt->setBackColor(TFT_WHITE);
    _dialog_txt->setTextColor(TFT_BLACK);
    _dialog_txt->setFontID(2);
    _dialog_txt->setTextSize(2);
    _dialog_txt->setPos(5, ctx_header_lbl->getHeight() + 5);
    _dialog_txt->setCornerRadius(3);

    _keyboard = creator.getStandardEnKeyboard(ID_KEYBOARD);
    layout->addWidget(_keyboard);
    _keyboard->setPos(0, _dialog_txt->getYPos() + _dialog_txt->getHeight() + 5);
}

void SetsEditorContext::clickOk()
{
    if (_mode == MODE_MAIN)
    {
        uint16_t id = _main_menu->getCurrItemID();
        if (id == ID_ITEM_CHANN)
        {
            _is_item_locked = true;
            _mode = MODE_SEL_CHANN;
            _main_menu->getCurrItem()->setBorderColor(TFT_RED);
        }
        else if (id == ID_ITEM_POWER)
        {
            _is_item_locked = true;
            _mode = MODE_SEL_POWER;
            _main_menu->getCurrItem()->setBorderColor(TFT_RED);
        }
        else if (id == ID_ITEM_ENC)
        {
            _lora_sets.encrypt_en = !_lora_sets.encrypt_en;
            ToggleItem *tog_item = _main_menu->getCurrItem()->castTo<ToggleItem>();
            tog_item->setOn(_lora_sets.encrypt_en);
        }
        else if (id == ID_ITEM_GEN_KEY)
        {
            generateAes256Key(_lora_sets.aes_key);
            showToast(STR_SUCCSESS);
        }
    }
    else if (_mode == MODE_ENTER_NAME)
    {
        _dialog_txt->addChars(_keyboard->getCurrBtnTxt().c_str());
    }
    else if (_mode == MODE_SEL_POWER || _mode == MODE_SEL_CHANN)
    {
        _is_item_locked = false;
        _mode = MODE_MAIN;
        _main_menu->getCurrItem()->setBorderColor(TFT_LIME);
    }
}

void SetsEditorContext::clickUp()
{
    if (_mode == MODE_MAIN)
    {
        _main_menu->focusUp();
        _main_scrollbar->scrollUp();
    }
    else if (_mode == MODE_ENTER_NAME)
    {
        _keyboard->focusUp();
    }
    else if (_mode == MODE_SEL_POWER)
    {
        ComboItem *power_item = _main_menu->getCurrItem()->castTo<ComboItem>();
        power_item->scrollUp();
        _lora_sets.power = power_item->getPos();
    }
    else if (_mode == MODE_SEL_CHANN)
    {
        SpinItem *spin_item = _main_menu->getCurrItem()->castTo<SpinItem>();
        spin_item->up();
        _lora_sets.channel = spin_item->getValue();
    }
}

void SetsEditorContext::clickDown()
{
    if (_mode == MODE_MAIN)
    {
        _main_menu->focusDown();
        _main_scrollbar->scrollDown();
    }
    else if (_mode == MODE_ENTER_NAME)
    {
        _keyboard->focusDown();
    }
    else if (_mode == MODE_SEL_POWER)
    {
        ComboItem *power_item = _main_menu->getCurrItem()->castTo<ComboItem>();
        power_item->scrollDown();
        _lora_sets.power = power_item->getPos();
    }
    else if (_mode == MODE_SEL_CHANN)
    {
        SpinItem *spin_item = _main_menu->getCurrItem()->castTo<SpinItem>();
        spin_item->down();
        _lora_sets.channel = spin_item->getValue();
    }
}

void SetsEditorContext::clickLeft()
{
    if (_mode == MODE_ENTER_NAME)
    {
        _keyboard->focusLeft();
    }
}

void SetsEditorContext::clickRight()
{
    if (_mode == MODE_ENTER_NAME)
    {
        _keyboard->focusRight();
    }
}

void SetsEditorContext::clickBack()
{
    if (_mode == MODE_ENTER_NAME)
    {
        _dialog_txt->removeLastChar();
    }
}

void SetsEditorContext::backPressed()
{
    if (_mode == MODE_ENTER_NAME)
    {
        showMainTmpl();
    }
    else
    {
        release();
    }
}

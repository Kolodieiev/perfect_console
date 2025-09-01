#include "PrefSelectContext.h"
#include "../WidgetCreator.h"

#include "meow/ui/widget/toggle/ToggleSwitch.h"
#include "meow/ui/widget/menu/item/ToggleItem.h"
//
#include "bright/PrefBrightContext.h"
#include "file_server/PrefFileServerContext.h"
#include "meow/manager/settings/SettingsManager.h"

void PrefSelectContext::showSDErrTmpl()
{
    _mode = MODE_SD_UNCONN;
    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    Label *err_lbl = new Label(ID_ERR_LBL);
    layout->addWidget(err_lbl);
    err_lbl->setText(STR_SD_ERR);
    err_lbl->setAlign(IWidget::ALIGN_CENTER);
    err_lbl->setGravity(IWidget::GRAVITY_CENTER);
    err_lbl->setBackColor(COLOR_MAIN_BACK);
    err_lbl->setWidth(D_WIDTH);
    err_lbl->setHeight(D_HEIGHT);
}

PrefSelectContext::PrefSelectContext()
{
    if (!_sd.isMounted())
    {
        showSDErrTmpl();
        return;
    }

    showMainTmpl();
}

void PrefSelectContext::showMainTmpl()
{
    WidgetCreator creator;

    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    _menu = new FixedMenu(ID_MENU);
    layout->addWidget(_menu);
    _menu->setBackColor(COLOR_MENU_ITEM);
    _menu->setWidth(D_WIDTH - SCROLLBAR_WIDTH - 2);
    _menu->setHeight(D_HEIGHT);
    _menu->setItemHeight((_menu->getHeight() - 2) / 5);

    //
    MenuItem *bright_item = creator.getMenuItem(ITEM_ID_BRIGHT);
    _menu->addItem(bright_item);
    Label *bright_lbl = creator.getItemLabel(STR_BRIGHT, 4, 2);
    bright_item->setLbl(bright_lbl);
    //
    ToggleItem *mono_item = new ToggleItem(ITEM_ID_AUDIO_MONO);
    _menu->addItem(mono_item);
    mono_item->setFocusBorderColor(TFT_LIME);
    mono_item->setFocusBackColor(COLOR_FOCUS_BACK);
    mono_item->setChangingBorder(true);
    mono_item->setChangingBack(true);

    Label *mono_lbl = creator.getItemLabel(STR_AUDIO_MONO, 4, 2);
    mono_item->setLbl(mono_lbl);

    ToggleSwitch *toggle_mono = new ToggleSwitch(ID_TOGGLE);
    mono_item->setToggle(toggle_mono);
    toggle_mono->setWidth(40);
    toggle_mono->setHeight(20);
    toggle_mono->setCornerRadius(7);

    String mono_mode = SettingsManager::get(STR_PREF_MONO_AUDIO);
    if (mono_mode.equals("") || mono_mode.equals("0"))
        toggle_mono->setOn(false);
    else
        toggle_mono->setOn(true);
    //
    MenuItem *file_server_item = creator.getMenuItem(ITEM_ID_FILE_SERVER);
    _menu->addItem(file_server_item);
    Label *file_server_lbl = creator.getItemLabel(STR_FILE_SERVER, 4, 2);
    file_server_item->setLbl(file_server_lbl);

    _scrollbar = new ScrollBar(ID_SCROLLBAR);
    layout->addWidget(_scrollbar);
    _scrollbar->setWidth(SCROLLBAR_WIDTH);
    _scrollbar->setHeight(D_HEIGHT);
    _scrollbar->setPos(D_WIDTH - SCROLLBAR_WIDTH, 0);
    _scrollbar->setMax(_menu->getSize());
}

bool PrefSelectContext::loop()
{
    if (_mode == MODE_SUBCONTEXT)
    {
        if (!_sub_context->isReleased())
        {
            _sub_context->tick();
            return false;
        }
        else
        {
            delete _sub_context;
            _sub_context = nullptr;
            _mode = MODE_NORMAL;
            showMainTmpl();
            return true;
        }
    }

    return true;
}

void PrefSelectContext::update()
{
    if (_mode == MODE_SD_UNCONN)
    {
        if (_input.isReleased(BtnID::BTN_BACK))
        {
            _input.lock(BtnID::BTN_BACK, CLICK_LOCK);
            openContextByID(ID_CONTEXT_MENU);
        }

        return;
    }

    if (_input.isReleased(BtnID::BTN_OK))
    {
        _input.lock(BtnID::BTN_OK, CLICK_LOCK);
        ok();
    }
    else if (_input.isReleased(BtnID::BTN_BACK))
    {
        _input.lock(BtnID::BTN_BACK, CLICK_LOCK);
        openContextByID(ID_CONTEXT_MENU);
    }
    else if (_input.isHolded(BtnID::BTN_UP))
    {
        _input.lock(BtnID::BTN_UP, HOLD_LOCK);
        _menu->focusUp();
        _scrollbar->scrollUp();
    }
    else if (_input.isHolded(BtnID::BTN_DOWN))
    {
        _input.lock(BtnID::BTN_DOWN, HOLD_LOCK);
        _menu->focusDown();
        _scrollbar->scrollDown();
    }
}

void PrefSelectContext::ok()
{
    uint16_t id = _menu->getCurrItemID();

    if (id == ITEM_ID_AUDIO_MONO)
    {
        ToggleItem *toggle = _menu->getCurrItem()->castTo<ToggleItem>();

        if (toggle->isOn())
        {
            if (SettingsManager::set(STR_PREF_MONO_AUDIO, "0"))
                toggle->setOn(false);
        }
        else
        {
            if (SettingsManager::set(STR_PREF_MONO_AUDIO, "1"))
                toggle->setOn(true);
        }
    }
    else if (id == ITEM_ID_BRIGHT)
    {
        _mode = MODE_SUBCONTEXT;
        getLayout()->delWidgets();
        _sub_context = new PrefBrightContext();
    }
    else if (id == ITEM_ID_FILE_SERVER)
    {
        _mode = MODE_SUBCONTEXT;
        getLayout()->delWidgets();
        _sub_context = new PrefFileServerContext();
    }
}

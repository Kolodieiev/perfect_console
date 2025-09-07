#include "LoraSetsContext.h"
#include "ExchangeSetsContext.h"
#include "SetsEditorContext.h"
#include "../WidgetCreator.h"
#include "ExchangeSetsContext.h"
#include "SetsEditorContext.h"

LoraSetsContext::LoraSetsContext()
{
    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    showMainTmpl();
    loadSetsList();
    fillSetsList();
}

LoraSetsContext::~LoraSetsContext()
{
}

bool LoraSetsContext::loop()
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
            _mode = MODE_MAIN;
            showMainTmpl();
            loadSetsList();
            fillSetsList();
            return true;
        }
    }

    return true;
}

void LoraSetsContext::update()
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
        showContextMenuTmpl();
    }
    else if (_input.isPressed(BtnID::BTN_BACK))
    {
        _input.lock(BtnID::BTN_BACK, PRESS_LOCK);
        release();
    }
}

void LoraSetsContext::clickOk()
{
    if (_mode == MODE_CTX_MENU)
    {
        uint16_t id = _context_menu->getCurrItemID();
        // TODO
        if (id == ID_CREATE_ITEM)
        {
            _mode = MODE_SUBCONTEXT;
            getLayout()->delWidgets();
            // _sub_context = new SetsEditorContext();
        }
        else if (id == ID_EDIT_ITEM)
        {
            _mode = MODE_SUBCONTEXT;
            getLayout()->delWidgets();
            // _sub_context = new SetsEditorContext();
        }
        else if (id == ID_GET_ITEM)
        {
            _mode = MODE_SUBCONTEXT;
            getLayout()->delWidgets();
            // _sub_context = new ExchangeSetsContext();
        }
        else if (id == ID_SHARE_ITEM)
        {
            _mode = MODE_SUBCONTEXT;
            getLayout()->delWidgets();
            // _sub_context = new ExchangeSetsContext();
        }
        else if (id == ID_USE_ITEM)
        {
            // Зберегти ім'я поточного налаштування
        }
        else if (id == ID_DELETE_ITEM)
        {
        }
    }
}

void LoraSetsContext::clickBack()
{
    if (_mode == MODE_CTX_MENU)
    {
        hideContextMenu();
    }
}

void LoraSetsContext::clickUp()
{
    if (_mode == MODE_MAIN)
    {
        _main_menu->focusUp();
        _main_scrollbar->scrollUp();
    }
    else if (_mode == MODE_CTX_MENU)
    {
        _context_menu->focusUp();
    }
}

void LoraSetsContext::clickDown()
{
    if (_mode == MODE_MAIN)
    {
        _main_menu->focusDown();
        _main_scrollbar->scrollDown();
    }
    else if (_mode == MODE_CTX_MENU)
    {
        _context_menu->focusDown();
    }
}

void LoraSetsContext::showMainTmpl()
{
    _mode = MODE_MAIN;

    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);
    //
    _main_menu = new FixedMenu(ID_MAIN_MENU);
    layout->addWidget(_main_menu);
    _main_menu->setBackColor(COLOR_MAIN_BACK);
    _main_menu->setWidth(_display.width() - SCROLLBAR_WIDTH - DISPLAY_PADDING * 2);
    _main_menu->setHeight(_display.height() - TFT_CUTOUT * 2);
    _main_menu->setItemHeight((_main_menu->getHeight() - 2) / 5);
    _main_menu->setPos(DISPLAY_PADDING, TFT_CUTOUT);
    //
    _main_scrollbar = new ScrollBar(ID_MAIN_SCROLL);
    layout->addWidget(_main_scrollbar);
    _main_scrollbar->setWidth(SCROLLBAR_WIDTH);
    _main_scrollbar->setHeight(_main_menu->getHeight());
    _main_scrollbar->setPos(_main_menu->getWidth() + _main_menu->getXPos(), _main_menu->getYPos());
    _main_scrollbar->setBackColor(COLOR_MAIN_BACK);
}

void LoraSetsContext::showContextMenuTmpl()
{
    if (_mode != MODE_MAIN)
        return;

    _mode = MODE_CTX_MENU;

    _main_menu->disable();

    WidgetCreator creator;

    _context_menu = new FixedMenu(ID_CTX_MENU);
    getLayout()->addWidget(_context_menu);
    _context_menu->setItemHeight(18);
    _context_menu->setWidth((float)D_WIDTH / 2);
    _context_menu->setBackColor(TFT_BLACK);
    _context_menu->setBorder(true);
    _context_menu->setBorderColor(TFT_ORANGE);
    _context_menu->setLoopState(true);

    // Створити
    MenuItem *create_item = creator.getMenuItem(ID_CREATE_ITEM);
    _context_menu->addItem(create_item);

    Label *create_lbl = creator.getItemLabel(STR_CREATE, 4);
    create_item->setLbl(create_lbl);
    create_lbl->setTextOffset(1);

    // TODO Отримати STR_GET

    if (_main_menu->getSize() > 0)
    {
        // Застосувати STR_APPLY
        // Редагувати STR_EDIT
        // Видалити STR_DELETE
        // Поділитися STR_SHARE
    }

    _context_menu->setHeight(_context_menu->getSize() * _context_menu->getItemHeight() + 4);
    _context_menu->setPos(D_WIDTH - _context_menu->getWidth() - DISPLAY_PADDING,
                          D_HEIGHT - DISPLAY_PADDING - _context_menu->getHeight());
}

void LoraSetsContext::hideContextMenu()
{
    getLayout()->delWidgetByID(ID_CTX_MENU);
    getLayout()->forcedDraw();
    _main_menu->enable();
    _mode = MODE_MAIN;
}

void LoraSetsContext::loadSetsList()
{
    String dir_name = SettingsManager::getSettingsDirPath(STR_LORA_SETS_DIR);

    if (dir_name.isEmpty())
        return;

    _fs.indexAll(_sets_files, dir_name.c_str());

    while (_fs.isWorking()) // Якщо хочеш, перепиши на асинхронне завантаження списку =)
        delay(1);
}

void LoraSetsContext::fillSetsList()
{
    WidgetCreator creator;
    _main_menu->delWidgets();

    size_t sets_num = _sets_files.size();
    uint16_t id_i = 0;

    for (size_t i = 0; i < sets_num; ++i)
    {
        ++id_i;

        MenuItem *item = creator.getMenuItem(id_i);
        _main_menu->addItem(item);

        Label *lbl = new Label(1);
        item->setLbl(lbl);
        lbl->setTickerInFocus(true);
        lbl->setText(_sets_files[i].getName());
    }

    _main_scrollbar->setValue(0);
    _main_scrollbar->setMax(_main_menu->getSize());
}

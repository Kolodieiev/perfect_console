#include "GamesListContext.h"
#include "../WidgetCreator.h"
#include "meow/ui/widget/layout/EmptyLayout.h"
#include "meow/ui/widget/menu/item/MenuItem.h"

#include "./icons/sokoban_ico.h"

const char STR_SOKOBAN_ITEM[] = "Комірник";
const char STR_TEST_SERV_ITEM[] = "Тестовий сервер";
const char STR_SIMPLE_RPG_ITEM[] = "Проста RPG";

GamesListContext::GamesListContext()
{
    //------ Налаштування зовнішнього вигляду віджетів

    WidgetCreator creator;
    //
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);
    //
    _menu = new FixedMenu(ID_MENU);
    layout->addWidget(_menu);
    _menu->setBackColor(COLOR_MENU_ITEM);
    _menu->setWidth(D_WIDTH - SCROLLBAR_WIDTH - 2);
    _menu->setHeight(D_HEIGHT - 2);
    _menu->setItemHeight((_menu->getHeight() - 2) / 4);
    //
    _scrollbar = new ScrollBar(ID_SCROLLBAR);
    layout->addWidget(_scrollbar);
    _scrollbar->setWidth(SCROLLBAR_WIDTH);
    _scrollbar->setHeight(D_HEIGHT);
    _scrollbar->setPos(D_WIDTH - SCROLLBAR_WIDTH, 0);
    //
    MenuItem *sokoban_item = creator.getMenuItem(ID_CONTEXT_SOKOBAN);
    _menu->addItem(sokoban_item);

    Image *soko_img = new Image(1);
    sokoban_item->setImg(soko_img);
    soko_img->init(32, 32);
    soko_img->setCornerRadius(5);
    soko_img->setTranspColor(TFT_TRANSPARENT);
    soko_img->setSrc(ICO_SOKOBAN);

    Label *soko_lbl = creator.getItemLabel(STR_SOKOBAN_ITEM, 4, 2);
    sokoban_item->setLbl(soko_lbl);
    soko_lbl->setTickerInFocus(true);

    //---------------------------------
    MenuItem *test_server_item = creator.getMenuItem(ID_CONTEXT_TEST_SERVER);
    _menu->addItem(test_server_item);

    Label *server_lbl = creator.getItemLabel(STR_TEST_SERV_ITEM, 4, 2);
    test_server_item->setLbl(server_lbl);
    server_lbl->setTickerInFocus(true);

    //---------------------------------
    MenuItem *simple_rpg_item = creator.getMenuItem(ID_CONTEXT_SIMPLE_RPG);
    _menu->addItem(simple_rpg_item);

    Label *simple_rpg_lbl = creator.getItemLabel(STR_SIMPLE_RPG_ITEM, 4, 2);
    simple_rpg_item->setLbl(simple_rpg_lbl);
    simple_rpg_lbl->setTickerInFocus(true);

    //---------------------------------
    _bin.reserve(_menu->getSize());
    _bin.push_back(soko_img);

    _scrollbar->setMax(_menu->getSize());
}

GamesListContext::~GamesListContext()
{
    for (auto b_it = _bin.begin(), e_it = _bin.end(); b_it != e_it; ++b_it)
        delete *b_it;
}

bool GamesListContext::loop()
{
    return true;
}

void GamesListContext::update()
{
    if (_input.isHolded(BtnID::BTN_UP))
    {
        _input.lock(BtnID::BTN_UP, HOLD_LOCK);
        up();
    }
    else if (_input.isHolded(BtnID::BTN_DOWN))
    {
        _input.lock(BtnID::BTN_DOWN, HOLD_LOCK);
        down();
    }
    else if (_input.isReleased(BtnID::BTN_BACK))
    {
        _input.lock(BtnID::BTN_BACK, CLICK_LOCK);
        openContextByID(ID_CONTEXT_MENU);
    }
    else if (_input.isReleased(BtnID::BTN_OK))
    {
        _input.lock(BtnID::BTN_OK, CLICK_LOCK);

        openContextByID((ContextID)_menu->getCurrItemID());
    }
}

void GamesListContext::up()
{
    _menu->focusUp();
    _scrollbar->scrollUp();
}

void GamesListContext::down()
{
    _menu->focusDown();
    _scrollbar->scrollDown();
}

void GamesListContext::ok()
{
    uint8_t context_id = _menu->getCurrItemID();
    openContextByID((ContextID)context_id); // Ігровий екран буде відкриватися без додаткової перевірки.
}

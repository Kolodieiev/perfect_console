#include "SokobanContext.h"
//
#include "../../WidgetCreator.h"
#include "./SceneID.h"
#include "scene/SokobanScene.h"

const char STR_LVL[] = "Рівень ";

namespace sokoban
{
    SokobanContext::SokobanContext()
    {
        WidgetCreator creator;
        EmptyLayout *layout = creator.getEmptyLayout();
        setLayout(layout);
        showLvlMenu();
    }

    bool SokobanContext::loop()
    {
        return true;
    }

    void SokobanContext::update()
    {
        if (_is_game_started)
        {
            if (!_scene->isFinished())
            {
                if (!_scene->isReleased())
                    _scene->update();
                else
                {
                    uint8_t next_lvl = _scene->getNextSceneID();
                    delete _scene;
                    _scene = new SokobanScene(_stored_objs, false, next_lvl);
                    _is_game_started = true;
                }
            }
            else
            {
                delete _scene;
                _is_game_started = false;
                showLvlMenu();
            }
        }
        else
        {
            if (_input.isReleased(BtnID::BTN_BACK))
            {
                _input.lock(BtnID::BTN_BACK, CLICK_LOCK);
                openContextByID(ID_CONTEXT_GAMES);
            }
            else if (_input.isHolded(BtnID::BTN_UP))
            {
                _input.lock(BtnID::BTN_UP, HOLD_LOCK);
                _lvl_menu->focusUp();
                _scrollbar->scrollUp();
            }
            else if (_input.isHolded(BtnID::BTN_DOWN))
            {
                _input.lock(BtnID::BTN_DOWN, HOLD_LOCK);
                _lvl_menu->focusDown();
                _scrollbar->scrollDown();
            }
            else if (_input.isReleased(BtnID::BTN_OK))
            {
                _input.lock(BtnID::BTN_OK, CLICK_LOCK);
                uint8_t next_lvl = _lvl_menu->getCurrItemID();
                getLayout()->delWidgets();
                _scene = new SokobanScene(_stored_objs, false, next_lvl);
                _is_game_started = true;
            }
        }
    }

    void SokobanContext::showLvlMenu()
    {
        WidgetCreator creator;
        EmptyLayout *layout = getLayout()->castTo<EmptyLayout>();

        _lvl_menu = new FixedMenu(ID_LVL_LIST);
        layout->addWidget(_lvl_menu);
        _lvl_menu->setBackColor(COLOR_MENU_ITEM);
        _lvl_menu->setWidth(D_WIDTH - SCROLLBAR_WIDTH);
        _lvl_menu->setHeight(D_HEIGHT);
        _lvl_menu->setItemHeight((D_HEIGHT - 2) / MENU_ITEMS_NUM);

        _scrollbar = new ScrollBar(ID_SCROLL);
        layout->addWidget(_scrollbar);
        _scrollbar->setWidth(SCROLLBAR_WIDTH);
        _scrollbar->setHeight(D_HEIGHT);
        _scrollbar->setPos(D_WIDTH - SCROLLBAR_WIDTH, 0);
        _scrollbar->setBackColor(COLOR_MAIN_BACK);

        for (uint8_t i{1}; i <= LEVEL_NUM; ++i)
        {
            MenuItem *lvl_item = creator.getMenuItem(i);
            _lvl_menu->addItem(lvl_item);

            String lbl = STR_LVL;
            lbl += i;

            Label *lvl_lbl = creator.getItemLabel(lbl.c_str());
            lvl_item->setLbl(lvl_lbl);
        }

        _scrollbar->setMax(_lvl_menu->getSize());
    }
}
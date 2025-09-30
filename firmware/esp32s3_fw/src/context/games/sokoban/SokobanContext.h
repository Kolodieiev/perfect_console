#pragma once
#include <Arduino.h>
//
#include "meow/game/DataStream.h"
//
#include "meow/ui/context/IContext.h"
#include "meow/ui/widget/menu/FixedMenu.h"
#include "meow/ui/widget/scrollbar/ScrollBar.h"
#include "meow/game/IGameScene.h"

using namespace meow;

namespace sokoban
{
    class SokobanContext : public IContext
    {
    public:
        SokobanContext();
        virtual ~SokobanContext() {}

    protected:
        virtual bool loop() override;
        virtual void update() override;

    private:
        enum Widget_ID : uint8_t
        {
            ID_LVL_LIST = 1,
            ID_SCROLL
        };

        void showLvlMenu();

    private:
        DataStream _stored_objs{1024};

        FixedMenu *_lvl_menu{nullptr};
        ScrollBar *_scrollbar{nullptr};
        IGameScene *_scene{nullptr};

        bool _is_game_started{false};
    };
}
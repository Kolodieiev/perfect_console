#pragma once
#include <Arduino.h>

#include "meow/ui/context/IContext.h"
#include "meow/ui/widget/menu/FixedMenu.h"
#include "meow/ui/widget/scrollbar/ScrollBar.h"

using namespace meow;

class GamesListContext : public IContext
{
public:
    GamesListContext();
    virtual ~GamesListContext();

protected:
    virtual bool loop() override;
    virtual void update() override;

private:
    enum Widget_ID : uint8_t
    {
        ID_MENU = 1,
        ID_SCROLLBAR,
    };

    void up();
    void down();

private:
    std::vector<IWidget *> _bin;
    FixedMenu *_menu;
    ScrollBar *_scrollbar;
};

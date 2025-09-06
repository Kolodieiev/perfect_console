#pragma once
#include <Arduino.h>
#include "meow/ui/context/IContext.h"
#include "meow/ui/widget/menu/FixedMenu.h"
#include "meow/ui/widget/scrollbar/ScrollBar.h"
#include "meow/manager/SettingsManager.h"

using namespace meow;

class LoraSetsContext : public IContext
{
public:
    LoraSetsContext();
    virtual ~LoraSetsContext();

protected:
    virtual bool loop() override;
    virtual void update() override;

private:
    enum Mode : uint8_t
    {
        MODE_MAIN = 0,
        MODE_CTX_MENU,
        MODE_SUBCONTEXT,
    };

    enum Widget_ID : uint8_t
    {
        ID_MAIN_MENU = 1,
        ID_MAIN_SCROLL,
        ID_CTX_MENU,
        //
        ID_CREATE_ITEM,
        ID_GET_ITEM,
        ID_USE_ITEM,
        ID_EDIT_ITEM,
        ID_DELETE_ITEM,
        ID_SHARE_ITEM,
    };

    void showMainTmpl();
    void showContextMenuTmpl();
    void hideContextMenu();
    //
    void loadSetsList();
    void fillSetsList();

    //
    void clickOk();
    void clickBack();
    void clickUp();
    void clickDown();

private:
    std::vector<FileInfo> _sets_files;

    IContext *_sub_context{nullptr};
    FixedMenu *_main_menu{nullptr};
    ScrollBar *_main_scrollbar{nullptr};
    FixedMenu *_context_menu{nullptr};

    Mode _mode{MODE_MAIN};
};

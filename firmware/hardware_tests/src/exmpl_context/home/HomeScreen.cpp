#include "HomeContext.h"
//
#include "../resources/color.h"
#include "../resources/const.h"
#include "../resources/string.h"
#include "../resources/ico/battery.h"
#include "./res/wall.h"
//
#include "../WidgetCreator.h"
#include "meow/ui/widget/layout/EmptyLayout.h"
//
#include "meow/util/img/BmpUtil.h"

HomeContext::HomeContext()
{
    WidgetCreator creator;

    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);
}

HomeContext::~HomeContext()
{
}

bool HomeContext::loop()
{
    return true;
}

void HomeContext::update()
{
    // if (_input.isReleased(BtnID::BTN_D))
    // {
    //     _input.lock(BtnID::BTN_D, 500);
    //     openContextByID(ID_CONTEXT_MENU);
    // }
}
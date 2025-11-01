#include "HomeContext.h"

#include "../WidgetCreator.h"

HomeContext::HomeContext()
{
  WidgetCreator creator;

  EmptyLayout* layout = creator.getEmptyLayout();
  setLayout(layout);

  Label* lbl = new Label(1);
  layout->addWidget(lbl);
  lbl->setText("Натисни Enter");
  lbl->setFont(font_10x20);
  lbl->setWidth(TFT_WIDTH);
  lbl->setHeight(TFT_HEIGHT);
  lbl->setGravity(IWidget::GRAVITY_CENTER);
  lbl->setAlign(IWidget::ALIGN_CENTER);
  lbl->setTextColor(COLOR_RED);
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
  if (_input.isReleased(BtnID::BTN_OK))
  {
    _input.lock(BtnID::BTN_OK, CLICK_LOCK);
    openContextByID(ContextID::ID_CONTEXT_MENU);
  }
}

#include "PrefSelectContext.h"

#include "../WidgetCreator.h"
#include "pixeler/src/widget/menu/item/ToggleItem.h"
#include "pixeler/src/widget/toggle/ToggleSwitch.h"
//
#include "bright/PrefBrightContext.h"
#include "file_server/PrefFileServerContext.h"
#include "pixeler/src/manager/SettingsManager.h"
#include "wifi_power/PrefWiFiPowerContext.h"

void PrefSelectContext::showSDErrTmpl()
{
  _mode = MODE_SD_UNCONN;

  EmptyLayout* layout = WidgetCreator::getEmptyLayout();
  setLayout(layout);

  Label* err_lbl = new Label(ID_ERR_LBL);
  layout->addWidget(err_lbl);
  err_lbl->setText(STR_SD_ERR);
  err_lbl->setAlign(IWidget::ALIGN_CENTER);
  err_lbl->setGravity(IWidget::GRAVITY_CENTER);
  err_lbl->setBackColor(COLOR_MAIN_BACK);
  err_lbl->setWidth(TFT_WIDTH);
  err_lbl->setHeight(TFT_HEIGHT);
}

PrefSelectContext::PrefSelectContext()
{
  if (!_fs.isMounted())
  {
    showSDErrTmpl();
    return;
  }

  showMainTmpl();
}

void PrefSelectContext::showMainTmpl()
{
  EmptyLayout* layout = WidgetCreator::getEmptyLayout();
  setLayout(layout);

  _menu = new FixedMenu(ID_MENU);
  layout->addWidget(_menu);
  _menu->setBackColor(COLOR_MENU_ITEM);
  _menu->setWidth(TFT_WIDTH - SCROLLBAR_WIDTH - 2);
  _menu->setHeight(TFT_HEIGHT);
  _menu->setItemHeight((_menu->getHeight() - 2) / 5);

  //
  MenuItem* bright_item = WidgetCreator::getMenuItem(ITEM_ID_BRIGHT);
  _menu->addItem(bright_item);
  Label* bright_lbl = WidgetCreator::getItemLabel(STR_BRIGHT, font_10x20);
  bright_item->setLbl(bright_lbl);
  //
  ToggleItem* mono_item = new ToggleItem(ITEM_ID_AUDIO_MONO);
  _menu->addItem(mono_item);
  mono_item->setFocusBorderColor(COLOR_LIME);
  mono_item->setFocusBackColor(COLOR_FOCUS_BACK);
  mono_item->setChangingBorder(true);
  mono_item->setChangingBack(true);

  Label* mono_lbl = WidgetCreator::getItemLabel(STR_AUDIO_MONO, font_10x20);
  mono_item->setLbl(mono_lbl);

  ToggleSwitch* toggle_mono = new ToggleSwitch(ID_TOGGLE);
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

  ToggleItem* audio_amp_item = mono_item->clone(ITEM_ID_AUDIO_AMP);
  _menu->addItem(audio_amp_item);
  audio_amp_item->getLbl()->setText(STR_AUDIO_AMP);

  String audio_amp_str = SettingsManager::get(STR_PREF_AUDIO_AMP);
  if (audio_amp_str.equals("") || audio_amp_str.equals("0"))
    audio_amp_item->getToggle()->setOn(false);
  else
    audio_amp_item->getToggle()->setOn(true);

  //

  ToggleItem* led_greet_item = mono_item->clone(ITEM_ID_LED_GREET);
  _menu->addItem(led_greet_item);
  led_greet_item->getLbl()->setText(STR_LED_GREET);

  String led_greet_str = SettingsManager::get(STR_PREF_LED_GREET);
  if (led_greet_str.equals("") || led_greet_str.equals("0"))
    led_greet_item->getToggle()->setOn(false);
  else
    led_greet_item->getToggle()->setOn(true);

  //

  MenuItem* file_server_item = WidgetCreator::getMenuItem(ITEM_ID_FILE_SERVER);
  _menu->addItem(file_server_item);
  Label* file_server_lbl = WidgetCreator::getItemLabel(STR_FILE_SERVER, font_10x20);
  file_server_item->setLbl(file_server_lbl);

  //

  MenuItem* wifi_power_item = WidgetCreator::getMenuItem(ITEM_ID_WIFI_POWER);
  _menu->addItem(wifi_power_item);
  Label* wifi_power_lbl = WidgetCreator::getItemLabel(STR_WIFI_POWER, font_10x20);
  wifi_power_item->setLbl(wifi_power_lbl);

  //

  _scrollbar = new ScrollBar(ID_SCROLLBAR);
  layout->addWidget(_scrollbar);
  _scrollbar->setWidth(SCROLLBAR_WIDTH);
  _scrollbar->setHeight(TFT_HEIGHT);
  _scrollbar->setPos(TFT_WIDTH - SCROLLBAR_WIDTH, 0);
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
    ToggleItem* toggle = _menu->getCurrItem()->castTo<ToggleItem>();

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
  else if (id == ITEM_ID_LED_GREET)
  {
    ToggleItem* toggle = _menu->getCurrItem()->castTo<ToggleItem>();

    if (toggle->isOn())
    {
      if (SettingsManager::set(STR_PREF_LED_GREET, "0"))
        toggle->setOn(false);
    }
    else
    {
      if (SettingsManager::set(STR_PREF_LED_GREET, "1"))
        toggle->setOn(true);
    }
  }
  else if (id == ITEM_ID_AUDIO_AMP)
  {
    ToggleItem* toggle = _menu->getCurrItem()->castTo<ToggleItem>();

    if (toggle->isOn())
    {
      if (SettingsManager::set(STR_PREF_AUDIO_AMP, "0"))
        toggle->setOn(false);
    }
    else
    {
      if (SettingsManager::set(STR_PREF_AUDIO_AMP, "1"))
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
  else if (id == ITEM_ID_WIFI_POWER)
  {
    _mode = MODE_SUBCONTEXT;
    getLayout()->delWidgets();
    _sub_context = new PrefWiFiPowerContext();
  }
}

#include "LoraSetsContext.h"

#include "../WidgetCreator.h"
#include "ExchangeSetsContext.h"
#include "SetsEditorContext.h"
#include "pixeler/util/string_util.h"

LoraSetsContext::LoraSetsContext()
{
  WidgetCreator creator;
  EmptyLayout* layout = creator.getEmptyLayout();
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

    if (id == ID_APPLY_ITEM)
    {
      hideContextMenu();

      String f_name = _main_menu->getCurrItemText();
      f_name += STR_LORA_SETS_EXT;

      if (!SettingsManager::set(STR_LORA_PRESET, f_name.c_str(), STR_LORA_SETS_DIR))
        showToast(STR_FAIL);
      else
        showToast(STR_SUCCESS);
    }
    else if (id == ID_DELETE_ITEM)
    {
      hideContextMenu();

      String file_path = SettingsManager::getSettingsFilePath(_main_menu->getCurrItemText().c_str(), STR_LORA_SETS_DIR);
      file_path += STR_LORA_SETS_EXT;

      if (!_fs.rmFile(file_path.c_str(), true))
      {
        showToast(STR_FAIL);
      }
      else
      {
        loadSetsList();
        showToast(STR_SUCCESS);
      }

      fillSetsList();
    }
    else
    {
      // Пункти, які відкривають субконтекст.

      _mode = MODE_SUBCONTEXT;

      String curr_item_txt = _main_menu->getCurrItemText();

      getLayout()->delWidgets();

      if (id == ID_CREATE_ITEM)
        _sub_context = new SetsEditorContext(emptyString);
      else if (id == ID_EDIT_ITEM)
        _sub_context = new SetsEditorContext(curr_item_txt);
      else if (id == ID_GET_ITEM)
        _sub_context = new ExchangeSetsContext(emptyString);
      else if (id == ID_SHARE_ITEM)
        _sub_context = new ExchangeSetsContext(curr_item_txt);
      else
      {
        log_e("Не реалізований пункт меню");
        esp_restart();
      }
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
  EmptyLayout* layout = creator.getEmptyLayout();
  setLayout(layout);
  //
  _main_menu = new FixedMenu(ID_MAIN_MENU);
  layout->addWidget(_main_menu);
  _main_menu->setBackColor(COLOR_MAIN_BACK);
  _main_menu->setWidth(TFT_WIDTH - SCROLLBAR_WIDTH - DISPLAY_PADDING * 2);
  _main_menu->setHeight(TFT_HEIGHT);
  _main_menu->setItemHeight((_main_menu->getHeight() - 2) / 5);
  _main_menu->setPos(DISPLAY_PADDING, 0);
  //
  _main_scrollbar = new ScrollBar(ID_MAIN_SCROLL);
  layout->addWidget(_main_scrollbar);
  _main_scrollbar->setWidth(SCROLLBAR_WIDTH);
  _main_scrollbar->setHeight(_main_menu->getHeight() - 4);
  _main_scrollbar->setPos(_main_menu->getWidth() + _main_menu->getXPos(), _main_menu->getYPos() + 2);
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
  _context_menu->setWidth((float)TFT_WIDTH / 2);
  _context_menu->setBackColor(COLOR_BLACK);
  _context_menu->setBorder(true);
  _context_menu->setBorderColor(COLOR_ORANGE);
  _context_menu->setLoopState(true);

  // Створити
  MenuItem* create_item = creator.getMenuItem(ID_CREATE_ITEM);
  _context_menu->addItem(create_item);

  Label* create_lbl = creator.getItemLabel(STR_CREATE, u8g2_font_unifont_t_cyrillic);
  create_item->setLbl(create_lbl);
  create_lbl->setHPaddings(1);

  // Отримати
  MenuItem* get_item = create_item->clone(ID_GET_ITEM);
  _context_menu->addItem(get_item);
  get_item->getLbl()->setText(STR_GET);

  if (_main_menu->getSize() > 0)
  {
    // Застосувати
    MenuItem* apply_item = create_item->clone(ID_APPLY_ITEM);
    _context_menu->addItem(apply_item);
    apply_item->getLbl()->setText(STR_APPLY);

    // Редагувати
    MenuItem* edit_item = create_item->clone(ID_EDIT_ITEM);
    _context_menu->addItem(edit_item);
    edit_item->getLbl()->setText(STR_EDIT);

    // Видалити
    MenuItem* delete_item = create_item->clone(ID_DELETE_ITEM);
    _context_menu->addItem(delete_item);
    delete_item->getLbl()->setText(STR_DELETE);

    // Поділитися
    MenuItem* share_item = create_item->clone(ID_SHARE_ITEM);
    _context_menu->addItem(share_item);
    share_item->getLbl()->setText(STR_SHARE);
  }

  _context_menu->setHeight(_context_menu->getSize() * _context_menu->getItemHeight() + 4);
  _context_menu->setPos(TFT_WIDTH - _context_menu->getWidth() - DISPLAY_PADDING,
                        TFT_HEIGHT - DISPLAY_PADDING - _context_menu->getHeight());
}

void LoraSetsContext::hideContextMenu()
{
  _mode = MODE_MAIN;
  _main_menu->enable();
  getLayout()->delWidgetByID(ID_CTX_MENU);
}

void LoraSetsContext::loadSetsList()
{
  String dirname = SettingsManager::getSettingsDirPath(STR_LORA_SETS_DIR);

  if (dirname.isEmpty())
    return;

  _fs.indexFilesExt(_sets_files, dirname.c_str(), STR_LORA_SETS_EXT);

  while (_fs.isWorking())  // Якщо хочеш, перепиши на асинхронне завантаження списку =)
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

    MenuItem* item = creator.getMenuItem(id_i);
    _main_menu->addItem(item);

    Label* lbl = new Label(1);
    item->setLbl(lbl);
    lbl->setFont(u8g2_font_unifont_t_cyrillic);
    lbl->setTextSize(2);
    lbl->setAutoscrollInFocus(true);

    String lbl_txt = _sets_files[i].getName();
    rmFilenameExt(lbl_txt, STR_LORA_SETS_EXT);
    lbl->setText(lbl_txt);
  }

  _main_scrollbar->setValue(0);
  _main_scrollbar->setMax(_main_menu->getSize());
}

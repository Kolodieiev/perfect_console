#include "SceneUI.h"

const char STR_WAIT_CLIENTS[] = "Очікуємо клієнтів";

namespace test_server
{
  SceneUI::SceneUI()
  {
  }

  void SceneUI::onDraw()
  {
    if (_is_msg_hide)
      return;

    _display.setTextColor(COLOR_CYAN);
    _display.setTextSize(2);
    _display.setFont(u8g2_font_unifont_t_cyrillic);
    _display.setCursor(25, 100);
    _display.print(STR_WAIT_CLIENTS);
  }

  void SceneUI::hideStartMsg()
  {
    _is_msg_hide = true;
  }
}  // namespace test_server
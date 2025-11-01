#pragma GCC optimize("O3")

#include "Input.h"

#include <stdexcept>

#include "soc/gpio_periph.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_struct.h"
#include "soc/io_mux_reg.h"

const char STR_UNKNOWN_PIN[] = "Незареєстрована віртуальна кнопка";

namespace pixeler
{
  Input::Input() {}

  void Input::__init()
  {
#ifdef EXT_INPUT
    _ext_input.init();
#endif  // EXT_INPUT

    _buttons = BUTTONS;
  }

  void Input::__update()
  {
#ifdef EXT_INPUT
    _ext_input.update();

    for (auto&& btn : _buttons)
      btn.second->__extUpdate(_ext_input.getBtnState(btn.first));
#else
    for (auto&& btn : _buttons)
      btn.second->__update();
#endif  // EXT_INPUT
  }

  void Input::reset()
  {
    for (auto&& btn : _buttons)
      btn.second->reset();
  }

  void Input::__printPinMode(BtnID pin_id)
  {
    if ((gpio_num_t)pin_id >= GPIO_NUM_MAX)
    {
      log_i("Invalid pin number: %d", pin_id);
      return;
    }

    uint32_t io_mux_reg = GPIO_PIN_MUX_REG[pin_id];  // Отримати адресу IOMUX регістра

    if (REG_GET_BIT(io_mux_reg, FUN_PU))
      log_i("Pin %d: Pull-up enabled", pin_id);
    else if (REG_GET_BIT(io_mux_reg, FUN_PD))
      log_i("Pin %d: Pull-down enabled", pin_id);
    else
      log_i("Pin %d: is floating", pin_id);
  }

  void Input::enableBtn(BtnID btn_id)
  {
#ifdef EXT_INPUT
    _ext_input.enableBtn(btn_id);
#else
    try
    {
      _buttons.at(btn_id)->enable();
    }
    catch (const std::out_of_range& ignored)
    {
      log_e("%s", STR_UNKNOWN_PIN);
    }
#endif  // EXT_INPUT
  }

  void Input::disableBtn(BtnID btn_id)
  {
#ifdef EXT_INPUT
    _ext_input.disableBtn(btn_id);
#else
    try
    {
      _buttons.at(btn_id)->disable();
    }
    catch (const std::out_of_range& ignored)
    {
      log_e("%s", STR_UNKNOWN_PIN);
    }
#endif  // EXT_INPUT
  }

  bool Input::isHolded(BtnID btn_id) const
  {
    try
    {
      return _buttons.at(btn_id)->isHolded();
    }
    catch (const std::out_of_range& ignored)
    {
      log_e("%s", STR_UNKNOWN_PIN);
      return false;
    }
  }

  bool Input::isPressed(BtnID btn_id) const
  {
    try
    {
      return _buttons.at(btn_id)->isPressed();
    }
    catch (const std::out_of_range& ignored)
    {
      log_e("%s", STR_UNKNOWN_PIN);
      return false;
    }
  }

  bool Input::isReleased(BtnID btn_id) const
  {
    try
    {
      return _buttons.at(btn_id)->isReleased();
    }
    catch (const std::out_of_range& ignored)
    {
      log_e("%s", STR_UNKNOWN_PIN);
      return false;
    }
  }

  void Input::lock(BtnID btn_id, unsigned long lock_duration)
  {
    try
    {
      _buttons.at(btn_id)->lock(lock_duration);
    }
    catch (const std::out_of_range& ignored)
    {
      log_e("%s", STR_UNKNOWN_PIN);
    }
  }

  Input _input;
}  // namespace pixeler
#pragma GCC optimize("O3")
#include "ContextManager.h"

#include "../driver/graphics/DisplayWrapper.h"
#include "../setup/context_id_setup.h"
#include "../setup/ui_setup.h"
#include "../ui/context/IContext.h"

namespace pixeler
{
  void ContextManager::run()
  {
    _input.__init();

    sf::RenderWindow _window{sf::VideoMode({TFT_WIDTH, TFT_HEIGHT}), WINDOW_TITLE};
    _display.init(&_window);

    IContext* _cur_context = new START_CONTEXT();

    unsigned long ts = millis();

    while (_window.isOpen())
    {
      while (auto event = _window.pollEvent())
      {
        if (event->is<sf::Event::Closed>())
        {
          log_i("Вікно було закрито. Вихід");
          _window.close();
          return;
        }
        else if (event->is<sf::Event::KeyPressed>())
        {
          if (event->getIf<sf::Event::KeyPressed>()->scancode == sf::Keyboard::Scancode::Up)
            _input.__setState(BtnID::BTN_UP, true);
          else if (event->getIf<sf::Event::KeyPressed>()->scancode == sf::Keyboard::Scancode::Down)
            _input.__setState(BtnID::BTN_DOWN, true);
          else if (event->getIf<sf::Event::KeyPressed>()->scancode == sf::Keyboard::Scancode::Left)
            _input.__setState(BtnID::BTN_LEFT, true);
          else if (event->getIf<sf::Event::KeyPressed>()->scancode == sf::Keyboard::Scancode::Right)
            _input.__setState(BtnID::BTN_RIGHT, true);
          else if (event->getIf<sf::Event::KeyPressed>()->scancode == sf::Keyboard::Scancode::Enter)
            _input.__setState(BtnID::BTN_OK, true);
          else if (event->getIf<sf::Event::KeyPressed>()->scancode == sf::Keyboard::Scancode::Backspace)
            _input.__setState(BtnID::BTN_BACK, true);
        }
        else if (event->is<sf::Event::KeyReleased>())
        {
          if (event->getIf<sf::Event::KeyReleased>()->scancode == sf::Keyboard::Scancode::Up)
            _input.__setState(BtnID::BTN_UP, false);
          else if (event->getIf<sf::Event::KeyReleased>()->scancode == sf::Keyboard::Scancode::Down)
            _input.__setState(BtnID::BTN_DOWN, false);
          else if (event->getIf<sf::Event::KeyReleased>()->scancode == sf::Keyboard::Scancode::Left)
            _input.__setState(BtnID::BTN_LEFT, false);
          else if (event->getIf<sf::Event::KeyReleased>()->scancode == sf::Keyboard::Scancode::Right)
            _input.__setState(BtnID::BTN_RIGHT, false);
          else if (event->getIf<sf::Event::KeyReleased>()->scancode == sf::Keyboard::Scancode::Enter)
            _input.__setState(BtnID::BTN_OK, false);
          else if (event->getIf<sf::Event::KeyReleased>()->scancode == sf::Keyboard::Scancode::Backspace)
            _input.__setState(BtnID::BTN_BACK, false);
        }
      }

      if (!_cur_context->isReleased())
      {
        _cur_context->tick();
      }
      else
      {
        ContextID next_context_id = _cur_context->getNextContextID();
        delete _cur_context;

        auto it = _context_id_map.find(next_context_id);
        if (it == _context_id_map.end())
        {
          log_e("Невідомий ідентифікатор контексту: %u", next_context_id);
          esp_restart();
        }
        else
        {
          _cur_context = it->second();
        }
      }

      if (millis() - ts > WDT_GUARD_TIME)
      {
        delay(1);
        ts = millis();
      }
    }
  }

}  // namespace pixeler
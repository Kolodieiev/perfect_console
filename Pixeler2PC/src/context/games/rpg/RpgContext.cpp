#include "RpgContext.h"

#include "../../WidgetCreator.h"
#include "./SceneID.h"
#include "scene/Rpg0Scene.h"

const char STR_LOAD_GAME[] = "Завантажити";
const char STR_NEW_GAME[] = "Нова гра";
const char STR_HELP[] = "Допомога";

namespace rpg
{
  RpgContext::~RpgContext()
  {
  }

  RpgContext::RpgContext()
  {
    WidgetCreator creator;
    EmptyLayout* layout = creator.getEmptyLayout();
    setLayout(layout);

    showGameMenuTmpl();
  }

  void RpgContext::showGameMenuTmpl()
  {
  }

  void RpgContext::showHelpTmpl()
  {
  }

  bool RpgContext::loop()
  {
    return true;
  }

  void RpgContext::update()
  {
    if (_is_game_started)
    {
      if (!_scene->isFinished())
      {
        if (!_scene->isReleased())
          _scene->update();
        else
        {
          uint8_t next_lvl = _scene->getNextSceneID();
          delete _scene;
          //   _scene = new SokobanScene(_stored_objs, false, next_lvl); // TODO
          //   _is_game_started = true;
        }
      }
      else
      {
        delete _scene;
        _is_game_started = false;
        showGameMenuTmpl();
      }
    }
    else
    {
      if (_input.isReleased(BtnID::BTN_BACK))
      {
        _input.lock(BtnID::BTN_BACK, CLICK_LOCK);
        openContextByID(ID_CONTEXT_GAMES);
      }
      else if (_input.isHolded(BtnID::BTN_UP))
      {
        _input.lock(BtnID::BTN_UP, HOLD_LOCK);
        _game_menu->focusUp();
      }
      else if (_input.isHolded(BtnID::BTN_DOWN))
      {
        _input.lock(BtnID::BTN_DOWN, HOLD_LOCK);
        _game_menu->focusDown();
      }
      else if (_input.isReleased(BtnID::BTN_OK))
      {
        _input.lock(BtnID::BTN_OK, CLICK_LOCK);
        // getLayout()->delWidgets();
        // _scene = new SokobanScene(_stored_objs, false, _prev_lvl); // TODO
        // _is_game_started = true;
      }
    }
  }

}  // namespace rpg
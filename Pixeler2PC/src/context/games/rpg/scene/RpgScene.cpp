#include "RpgScene.h"

#include "../ResID.h"
#include "../SceneID.h"
#include "../ui/SceneUI.h"

namespace rpg
{
  RpgScene::RpgScene(DataStream& stored_objs, bool is_loaded, uint8_t lvl) : IGameScene(stored_objs)
  {
    _level = --lvl;

    // TODO Додати завантаження ресурсів в PSRAM з диску по їх ідентифікатору.
    // TODO Додати прогрес завантаження ресурсів

    buildTerrain();
    createHero();

    _game_UI = new SceneUI();
  }

  RpgScene::~RpgScene()
  {
  }

  void RpgScene::update()
  {
    if (_input.isPressed(BtnID::BTN_OK))
    {
      // show ingame menu
      return;
    }
    else if (_input.isPressed(BtnID::BTN_BACK))
    {
      _input.lock(BtnID::BTN_BACK, PRESS_LOCK);
      _is_finished = true;
      return;
    }
    else if (_input.isReleased(BtnID::BTN_OK))
    {
      _input.lock(BtnID::BTN_OK, CLICK_LOCK);
      // якщо в сцені - переміщення, якщо в ігровому меню, обробка пункту меню
    }
    else
    {
      IGameObject::MovingDirection direction = IGameObject::DIRECTION_NONE;

      if (_input.isReleased(BtnID::BTN_UP))
      {
        _input.lock(BtnID::BTN_UP, 100);
        direction = IGameObject::DIRECTION_UP;
      }
      else if (_input.isReleased(BtnID::BTN_DOWN))
      {
        _input.lock(BtnID::BTN_DOWN, 100);
        direction = IGameObject::DIRECTION_DOWN;
      }
      else if (_input.isReleased(BtnID::BTN_RIGHT))
      {
        _input.lock(BtnID::BTN_RIGHT, 100);
        direction = IGameObject::DIRECTION_RIGHT;
      }
      else if (_input.isReleased(BtnID::BTN_LEFT))
      {
        _input.lock(BtnID::BTN_LEFT, 100);
        direction = IGameObject::DIRECTION_LEFT;
      }

      // _hero_obj->move(direction); TODO
    }
  }

  void RpgScene::onTrigger(uint8_t id)
  {
  }

  void RpgScene::buildTerrain()
  {
  }

  void RpgScene::createHero()
  {
    // _main_obj =
  }

}  // namespace rpg

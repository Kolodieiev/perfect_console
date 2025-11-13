#include "RpgBaseScene.h"

#include "../ResID.h"
#include "../SceneID.h"
#include "../map/map_scene_0.h"
#include "../map/map_scene_1.h"
#include "../map/map_scene_2.h"
#include "../map/map_scene_3.h"
#include "../ui/SceneUI.h"

namespace rpg
{
  const char STR_MAPS_RES_FOLDER[] = "games/rpg/tiles/map/";

    const MapInfo MAPS_INFO[] = {
      {MAP_SCENE_0_W, MAP_SCENE_0_H, MAP_SCENE_0},
      {MAP_SCENE_1_W, MAP_SCENE_1_H, MAP_SCENE_1},
      {MAP_SCENE_2_W, MAP_SCENE_2_H, MAP_SCENE_2},
      {MAP_SCENE_3_W, MAP_SCENE_3_H, MAP_SCENE_3},
  };

  RpgBaseScene::RpgBaseScene(DataStream& stored_objs, bool is_loaded, uint8_t lvl) : IGameScene(stored_objs)
  {
    _level = --lvl;

    if (!loadMapRes())
    {
      _has_error = true;
      // TODO show err notification
      log_e("Помилка завантаження ресурсів");
      return;
    }

    log_e("Ресурси завантажено");

    buildTerrain();
    createHero();

    _game_UI = new SceneUI();
  }

  RpgBaseScene::~RpgBaseScene()
  {
    clearMapRes();
  }

  void RpgBaseScene::update()
  {
    if (_has_error)
    {
      if (_input.isReleased(BtnID::BTN_OK))
      {
        _input.lock(BtnID::BTN_OK, CLICK_LOCK);
        _is_finished = true;
      }
      return;
    }

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

  void RpgBaseScene::onTrigger(uint8_t id)
  {
  }

  void RpgBaseScene::clearMapRes()
  {
    for (auto&& res : _map_resources)
      _res_manager.deleteBmpRes(res.second);

    _map_resources.clear();
  }

  bool RpgBaseScene::loadMapRes()
  {
    // TODO Додати прогрес завантаження ресурсів
    MapInfo map_info = MAPS_INFO[_level];

    uint16_t map_size = map_info.width * map_info.height;

    for (uint16_t i = 0; i < map_size; ++i)
    {
      if (_map_resources.find(map_info.map_tmpl[i]) != _map_resources.end())
        continue;

      String cur_file_name = STR_MAPS_RES_FOLDER;
      cur_file_name += map_info.map_tmpl[i];
      cur_file_name += ".bmp";

      uint16_t res_id = _res_manager.loadBmpRes(cur_file_name.c_str());

      if (res_id == 0)
        return false;

      log_i("Завантажено %u", map_info.map_tmpl[i]);

      _map_resources.emplace(map_info.map_tmpl[i], res_id);
    }

    return true;
  }

  void RpgBaseScene::buildTerrain()
  {
  }

  void RpgBaseScene::createHero()
  {
    // _main_obj =
  }

}  // namespace rpg

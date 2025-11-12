#include "RpgScene.h"

#include "../ResID.h"
#include "../SceneID.h"
#include "../ui/SceneUI.h"

namespace rpg
{
  RpgScene::RpgScene(DataStream& stored_objs, bool is_loaded, uint8_t lvl) : IGameScene(stored_objs)
  {
    _level = --lvl;

    _game_UI = new SceneUI();
  }

  RpgScene::~RpgScene()
  {
  }

  void RpgScene::update()
  {
  }

  void RpgScene::onTrigger(uint8_t id)
  {
  }

  void RpgScene::buildTerrain()
  {
  }

  void RpgScene::createHero()
  {
  }

}  // namespace rpg

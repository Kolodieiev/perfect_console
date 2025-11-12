#pragma once

#include "pixeler/game/IGameScene.h"

using namespace pixeler;

namespace rpg
{
  class RpgScene : public IGameScene
  {
  public:
    explicit RpgScene(DataStream& stored_objs, bool is_loaded = false, uint8_t lvl = 1);
    virtual ~RpgScene();
    virtual void update() override;
    virtual void onTrigger(uint8_t id) override;

  private:
    void buildTerrain();
    void createHero();

  private:
    uint8_t _level{0};
  };

}  // namespace rpg

#pragma once

#include "pixeler/game/IGameScene.h"

using namespace pixeler;

namespace rpg
{
  class RpgBaseScene : public IGameScene
  {
  public:
    explicit RpgBaseScene(DataStream& stored_objs, bool is_loaded = false, uint8_t lvl = 1);
    virtual ~RpgBaseScene();
    virtual void update() override;
    virtual void onTrigger(uint8_t id) override;

  private:
    void clearMapRes();
    bool loadMapRes();
    void buildTerrain();
    void createHero();

  private:
    std::unordered_map<uint16_t, uint16_t> _map_resources;

    String _err_message;
    bool _has_error{false};
    uint8_t _level{0};
  };

}  // namespace rpg

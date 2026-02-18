#pragma once
#include <stdint.h>

#include "pixeler/src/game/object/IGameObject.h"

using namespace pixeler;

namespace simple_rpg
{
  class BunnyObj : public IGameObject
  {
  public:
    BunnyObj(WavManager& audio,
             TerrainManager& terrain,
             std::unordered_map<uint32_t, IGameObject*>& game_objs) : IGameObject(audio, terrain, game_objs) {}
    virtual ~BunnyObj() {}

    virtual void init() override;
    virtual void update() override;
    //
    virtual void serialize(DataStream& ds) override {}
    virtual void deserialize(DataStream& ds) override {}
    virtual size_t getDataSize() const override
    {
      return 0;
    }

  private:
    void initAnimVectors();
    void disableAnimation();

  private:
    std::vector<const uint16_t*> move_up_anim;
    std::vector<const uint16_t*> move_down_anim;
    std::vector<const uint16_t*> move_right_anim;
    std::vector<const uint16_t*> move_left_anim;

    MovingDirection _prev_direction{DIRECTION_NONE};
  };
}  // namespace simple_rpg
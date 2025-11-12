#pragma once

#include "pixeler/game/object/IGameObject.h"

using namespace pixeler;

namespace rpg
{
  class HeroObj : public IGameObject
  {
  public:
    HeroObj(ResManager& res,
            WavManager& audio,
            TerrainManager& terrain,
            std::unordered_map<uint32_t, IGameObject*>& game_objs);

    virtual ~HeroObj();

    virtual void init() override;
    virtual void update() override;
    virtual void serialize(DataStream& ds) override;
    virtual void deserialize(DataStream& ds) override;
    virtual size_t getDataSize() const override;

    virtual void onDraw() override;
    void move(MovingDirection direction);

  private:
  };

};  // namespace rpg
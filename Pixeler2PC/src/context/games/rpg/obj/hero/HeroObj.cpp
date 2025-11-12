#include "HeroObj.h"

namespace rpg
{

  HeroObj::HeroObj(ResManager& res, WavManager& audio, TerrainManager& terrain, std::unordered_map<uint32_t, IGameObject*>& game_objs)
      : IGameObject(res, audio, terrain, game_objs)
  {
  }

  HeroObj::~HeroObj()
  {
  }

  void HeroObj::init()
  {
  }

  void HeroObj::update()
  {
  }

  void HeroObj::serialize(DataStream& ds)
  {
  }

  void HeroObj::deserialize(DataStream& ds)
  {
  }

  size_t HeroObj::getDataSize() const
  {
    return size_t();
  }

  void HeroObj::onDraw()
  {
  }

  void HeroObj::move(MovingDirection direction)
  {
  }

};  // namespace rpg
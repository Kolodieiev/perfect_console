#pragma once

#include "../BallObj.h"

using namespace pixeler;

namespace test_server
{
  class GreenBallObj : public BallObj
  {
  public:
    GreenBallObj(WavManager& audio,
                 TerrainManager& terrain,
                 std::unordered_map<uint32_t, IGameObject*>& game_objs) : BallObj(audio, terrain, game_objs) {}
    virtual ~GreenBallObj() override {}

    virtual void init() override;

  private:
  };
}  // namespace test_server
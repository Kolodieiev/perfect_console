#pragma once

#include "../BallObj.h"

using namespace pixeler;

namespace test_server
{
  class GreenBallObj : public BallObj
  {
  public:
    GreenBallObj(ResManager& res,
                 WavManager& audio,
                 TerrainManager& terrain,
                 std::unordered_map<uint32_t, IGameObject*>& game_objs) : BallObj(res, audio, terrain, game_objs) {}
    virtual ~GreenBallObj() override {}

    virtual void init() override;

  private:
  };
}  // namespace test_server
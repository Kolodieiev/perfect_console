#pragma once

#include "pixeler/game/IGameUI.h"
#include "pixeler/game/object/IGameObject.h"

using namespace pixeler;

namespace sokoban
{
  class SceneUI : public IGameUI
  {
  public:
    SceneUI() {}
    virtual ~SceneUI() {}
    virtual void onDraw() override;

  private:
  };
}  // namespace sokoban
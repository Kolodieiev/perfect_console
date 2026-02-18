#pragma once

#include "pixeler/src/game/IGameUI.h"
#include "pixeler/src/game/object/IGameObject.h"

using namespace pixeler;

namespace test_server
{
  class SceneUI : public IGameUI
  {
  public:
    SceneUI();
    virtual ~SceneUI() {}
    virtual void onDraw() override;
    void hideStartMsg();

  private:
    bool _is_msg_hide{false};
  };
}  // namespace test_server
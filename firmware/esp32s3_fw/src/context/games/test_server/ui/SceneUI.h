#pragma once
#include <Arduino.h>

#include "pixeler/game/IGameUI.h"
#include "pixeler/game/object/IGameObject.h"

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
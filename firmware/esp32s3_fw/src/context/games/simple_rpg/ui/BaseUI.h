#pragma once

#include "pixeler/game/IGameUI.h"
#include "pixeler/ui/widget/text/Label.h"

using namespace pixeler;

namespace simple_rpg
{
  class BaseUI : public IGameUI
  {
  public:
    BaseUI();
    virtual ~BaseUI()
    {
      delete _bunny_counter_lbl;
    }

    virtual void onDraw() override;
    void updBunnyNum(uint16_t value);

  private:
    Label* _bunny_counter_lbl{nullptr};
  };
}  // namespace simple_rpg
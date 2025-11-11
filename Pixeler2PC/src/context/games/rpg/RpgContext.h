#pragma once
//
#include "pixeler/game/DataStream.h"
//
#include "pixeler/game/IGameScene.h"
#include "pixeler/ui/context/IContext.h"
#include "pixeler/ui/widget/menu/FixedMenu.h"

using namespace pixeler;

namespace rpg
{
  class RpgContext : public IContext
  {
  public:
    RpgContext();
    virtual ~RpgContext();

  protected:
    virtual bool loop() override;
    virtual void update() override;

  private:
    enum Widget_ID : uint8_t
    {
      ID_LVL_LIST = 1,
    };

    void showGameMenuTmpl();
    void showHelpTmpl();

    DataStream _stored_objs{1024};

    FixedMenu* _game_menu{nullptr};
    IGameScene* _scene{nullptr};

    bool _is_game_started{false};
  };

}  // namespace rpg
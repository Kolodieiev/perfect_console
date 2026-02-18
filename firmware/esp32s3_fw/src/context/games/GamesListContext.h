#pragma once
#include <stdint.h>

#include "pixeler/context/IContext.h"
#include "pixeler/widget/menu/FixedMenu.h"
#include "pixeler/widget/scrollbar/ScrollBar.h"

using namespace pixeler;

class GamesListContext : public IContext
{
public:
  GamesListContext();
  virtual ~GamesListContext();

protected:
  virtual bool loop() override;
  virtual void update() override;

private:
  enum Widget_ID : uint8_t
  {
    ID_MENU = 1,
    ID_SCROLLBAR,
  };

  void up();
  void down();

private:
  FixedMenu* _menu;
  ScrollBar* _scrollbar;
};

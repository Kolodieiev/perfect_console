#pragma once
#include <Arduino.h>

#include "pixeler/ui/context/IContext.h"
#include "pixeler/ui/widget/menu/FixedMenu.h"
#include "pixeler/ui/widget/scrollbar/ScrollBar.h"

using namespace pixeler;

class PrefSelectContext : public IContext
{
public:
  PrefSelectContext();
  virtual ~PrefSelectContext() {}

protected:
  virtual bool loop() override;
  virtual void update() override;

private:
  enum Widget_ID : uint8_t
  {
    ID_MENU = 2,
    ID_SCROLLBAR,
    ID_TOGGLE,
    ID_ERR_LBL,
  };

  enum ItemID : uint8_t
  {
    ITEM_ID_BRIGHT = 1,
    ITEM_ID_AUDIO_MONO,
    ITEM_ID_AUDIO_AMP,
    ITEM_ID_WATCH,
    ITEM_ID_FILE_SERVER,
  };

  enum Mode : uint8_t
  {
    MODE_NORMAL = 0,
    MODE_SD_UNCONN,
    MODE_SUBCONTEXT,
  };

  Mode _mode = MODE_NORMAL;

  FixedMenu* _menu;
  ScrollBar* _scrollbar;

  IContext* _sub_context{nullptr};

  void ok();
  void showSDErrTmpl();
  void showMainTmpl();
};
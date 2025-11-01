#pragma once

#include "../../pixeler/ui/context/IContext.h"
#include "../../pixeler/ui/widget/text/Label.h"

using namespace pixeler;

class HomeContext : public IContext
{
public:
  HomeContext();
  virtual ~HomeContext();

protected:
  virtual bool loop() override;
  virtual void update() override;
  //
private:
  enum Widget_ID : uint8_t
  {
    ID_WALLPAPER = 1,
    ID_BAT_LVL,
    ID_TIME_LBL,
    ID_DATE_LBL,
    ID_DAY_LBL,
  };

};

#pragma GCC optimize("O3")
#include "ContextManager.h"

#include "pixeler/manager/SPI_Manager.h"
#include "pixeler/ui/context/IContext.h"
#include "../setup/context_id_setup.h"
#include "../setup/display_setup.h"
#include "../setup/ui_setup.h"


namespace pixeler
{
  void ContextManager::run()
  {
    _input.__init();

#ifdef GRAPHICS_ENABLED
    _display.init();
#endif

    IContext* context = new START_CONTEXT();

    unsigned long ts = millis();
    while (1)
    {
      if (!context->isReleased())
      {
        context->tick();
      }
      else
      {
        ContextID next_context_id = context->getNextContextID();
        delete context;

        auto it = _context_id_map.find(next_context_id);
        if (it == _context_id_map.end())
        {
          log_e("Невідомий ідентифікатор контексту: %u", next_context_id);
          esp_restart();
        }
        else
        {
          context = it->second();
        }
      }

      if (millis() - ts > WDT_GUARD_TIME)
      {
        delay(1);
        ts = millis();
      }
    }
  }
}  // namespace pixeler
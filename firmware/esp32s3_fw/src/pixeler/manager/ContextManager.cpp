#pragma GCC optimize("O3")
#include "ContextManager.h"

#include "pixeler/manager/SPI_Manager.h"
#include "pixeler/ui/context/IContext.h"
#include "pixeler_setup/context_id.h"
#include "pixeler_setup/display_setup.h"
#include "pixeler_setup/ui_setup.h"

#define WD_GUARD_TIME 1000UL

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

      if (millis() - ts > WD_GUARD_TIME)
      {
        vTaskDelay(1 / portTICK_PERIOD_MS);
        ts = millis();
      }
    }
  }
}  // namespace pixeler
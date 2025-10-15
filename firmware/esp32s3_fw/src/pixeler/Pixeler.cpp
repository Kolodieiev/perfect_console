#include "Pixeler.h"

#include "pixeler/manager/ContextManager.h"

namespace pixeler
{
  void Pixeler::begin(uint32_t stack_depth_kb)
  {
    xTaskCreatePinnedToCore(pixelerContextTask, "mct", stack_depth_kb * 512, NULL, 10, NULL, 1);
  }

  void Pixeler::pixelerContextTask(void* params)
  {
    ContextManager context_manager;
    context_manager.run();
  }

  Pixeler PIXELER;
}  // namespace pixeler
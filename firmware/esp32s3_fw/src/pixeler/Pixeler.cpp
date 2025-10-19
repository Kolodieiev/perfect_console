#include "Pixeler.h"

#include <pixeler_setup/cpu_setup.h>

#include "pixeler/manager/ContextManager.h"

namespace pixeler
{
  void Pixeler::begin(uint32_t stack_depth_kb)
  {
    xTaskCreatePinnedToCore(pixelerContextTask, "mct", stack_depth_kb * 512, NULL, 10, NULL, 1);
  }

  void Pixeler::pixelerContextTask(void* params)
  {
    setCpuFrequencyMhz(BASE_CPU_FREQ_MHZ);

    ContextManager context_manager;
    context_manager.run();
  }

  Pixeler PIXELER;
}  // namespace pixeler
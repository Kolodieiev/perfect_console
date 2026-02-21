#pragma once
#pragma GCC optimize("O3")

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

namespace pixeler
{
  class MutexGuard
  {
  public:
    explicit MutexGuard(SemaphoreHandle_t mutex)
        : _mutex(mutex)
    {
      assert(mutex);
      assert(xSemaphoreTake(mutex, portMAX_DELAY) == pdTRUE);
    }

    ~MutexGuard()
    {
      xSemaphoreGive(_mutex);
    }

    MutexGuard(const MutexGuard&) = delete;
    MutexGuard& operator=(const MutexGuard&) = delete;

  private:
    SemaphoreHandle_t _mutex;
  };
}  // namespace pixeler
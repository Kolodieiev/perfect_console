#pragma once
#pragma GCC optimize("O3")

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

namespace pixeler
{
  class AutoLock
  {
  public:
    explicit AutoLock(SemaphoreHandle_t mutex)
        : _mutex(mutex)
    {
      xSemaphoreTake(_mutex, portMAX_DELAY);
    }

    ~AutoLock()
    {
      xSemaphoreGive(_mutex);
    }

    AutoLock(const AutoLock&) = delete;
    AutoLock& operator=(const AutoLock&) = delete;

  private:
    SemaphoreHandle_t _mutex;
  };
}  // namespace pixeler
#pragma once

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef ESP32
#include <Arduino.h>
#else
#include <chrono>
#include <functional>
#include <mutex>
#include <thread>

#include "String/WString.h"

//--------------------------------------------------------------------------

using SemaphoreHandle_t = std::mutex*;
constexpr unsigned long portMAX_DELAY = (unsigned long)-1;

inline SemaphoreHandle_t xSemaphoreCreateMutex()
{
  return new std::mutex();
}

inline void vSemaphoreDelete(SemaphoreHandle_t handle)
{
  delete handle;
}

inline bool xSemaphoreTake(SemaphoreHandle_t handle, unsigned long timeout_ms)
{
  using namespace std::chrono;

  if (timeout_ms == portMAX_DELAY)
  {
    handle->lock();
    return true;
  }

  auto start = steady_clock::now();
  while (!handle->try_lock())
  {
    if (duration_cast<milliseconds>(steady_clock::now() - start).count() >= timeout_ms)
      return false;
    std::this_thread::sleep_for(milliseconds(1));
  }
  return true;
}

inline void xSemaphoreGive(SemaphoreHandle_t handle)
{
  handle->unlock();
}

//--------------------------------------------------------------------------

#define log_e(fmt, ...) printf("E: " fmt "\n", ##__VA_ARGS__)
#define log_i(fmt, ...) printf("I: " fmt "\n", ##__VA_ARGS__)

//--------------------------------------------------------------------------

#define esp_restart() exit(1)

//--------------------------------------------------------------------------

#define delay(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))

#define millis() (std::chrono::duration_cast<std::chrono::milliseconds>(   \
                      std::chrono::steady_clock::now().time_since_epoch()) \
                      .count())

#endif  // #ifdef ESP32

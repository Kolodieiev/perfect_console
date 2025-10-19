#pragma once
#include <Arduino.h>

namespace pixeler
{
  struct AudioData
  {
    uint32_t size{0};
    uint8_t* data_ptr{nullptr};
  };
}  // namespace pixeler
#pragma once

#include <stdint.h>

namespace pixeler
{
  struct MapInfo
  {
    uint16_t width{0};                  // Ширина мапи.
    uint16_t height{0};                 // Висота мапи.
    const uint16_t* map_tmpl{nullptr};  // Вказівник на масив з шаблоном мапи.
  };
};  // namespace pixeler

#pragma once
#pragma GCC optimize("O3")
#include "../defines.h"

namespace pixeler
{
  /**
   * @brief Обрізає розширення файлу в рядку.
   *
   * @param filename Рядок, в якому потрібно видалити розширення.
   */
  void rmFilenameExt(String& filename);
}  // namespace pixeler
#pragma once
#include "defines.h"

namespace pixeler
{
  class Pixeler
  {
  public:
    /**
     * @brief Виконує початкову ініціалізацію фреймворку та запускає задачу менеджера контекстів.
     *
     * @param stack_depth_kb Розмір стеку Pixeler в кілобайтах.
     * Пам'ять цього стеку буде використовуватися на потреби фреймворку, а також під час виконання будь-якого із контекстів.
     */
    void begin(uint32_t stack_depth_kb);

  private:
    static void pixelerContextTask(void* params);
  };

  extern Pixeler PIXELER;
}  // namespace pixeler

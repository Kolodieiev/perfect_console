// Перечислення, в якому зберігаються ідентифікатори типів ігрових об'єктів
#pragma once
#include <stdint.h>

namespace test_server
{
  enum ClassID : uint8_t
  {
    GREEN_BALL = 0,
    BLUE_BALL,
  };
}
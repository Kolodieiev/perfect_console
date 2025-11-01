#include <windows.h>

#include <cstdint>

#include "pixeler/manager/ContextManager.h"

#define CP_UTF8 65001
void activate_utf8_on_windows()
{
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
}

using namespace pixeler;

int main()
{
  activate_utf8_on_windows();

  ContextManager context_manager;
  context_manager.run();

  return 0;
}

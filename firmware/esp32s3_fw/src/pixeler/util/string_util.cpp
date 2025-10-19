#pragma GCC optimize("O3")
#include "string_util.h"

namespace pixeler
{
  void rmFilenameExt(String& out_str, const char* ext)
  {
    int pos = out_str.indexOf(ext);
    if (pos != -1)
      out_str.remove(pos);
  }
}  // namespace pixeler
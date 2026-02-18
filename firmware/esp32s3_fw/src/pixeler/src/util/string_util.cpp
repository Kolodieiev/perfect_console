#pragma GCC optimize("O3")
#include "string_util.h"

namespace pixeler
{
  void rmFilenameExt(String& filename)
  {
    int pos = filename.lastIndexOf(".");
    if (pos != -1)
      filename.remove(pos);
  }
}  // namespace pixeler
#pragma once
#pragma GCC optimize("O3")
#include <stdint.h>

#include "./AudioData.h"
#include "./WavHeader.h"

namespace pixeler
{
  class WavUtil
  {
  public:
    static AudioData loadWav(const char* path_to_wav);

  private:
    static bool validateHeader(const WavHeader& wav_header);
  };
}  // namespace pixeler
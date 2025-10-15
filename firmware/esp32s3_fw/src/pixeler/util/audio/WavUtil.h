#pragma once
#pragma GCC optimize("O3")
#include <Arduino.h>

#include "./AudioData.h"
#include "./WavHeader.h"

namespace pixeler
{
  class WavUtil
  {
  public:
    AudioData loadWav(const char* path_to_wav);

  private:
    bool validateHeader(const WavHeader& wav_header);
  };
}  // namespace pixeler
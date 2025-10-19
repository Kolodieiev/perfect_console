#pragma once
#include <stdint.h>

#pragma pack(push, 1)

struct CodecSettings
{
  float hpf_out_freq{150.0f};
  float agc_out_db{-7.0f};
  float agc_in_db{-7.0f};

  float post_filter_beta{1.0f};
  float post_filter_gamma{0.4f};
  int volume{1};

  bool agc_in_en{false};
  bool post_filter_en{true};
  bool post_filter_bboost_en{true};
  bool interleaving_en{true};
};

#pragma pack(pop)
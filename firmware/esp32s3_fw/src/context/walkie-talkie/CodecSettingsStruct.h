#pragma once
#include <stdint.h>

#pragma pack(push, 1)

struct CodecSettings
{
    float hpf_out_freq{200.0f};
    float agc_out_db{-7.0f};
    float agc_in_db{-7.0f};

    float post_filter_beta{0.6f};
    float post_filter_gamma{0.4f};

    bool agc_in_en{false};
    bool post_filter_en{true};
    bool post_filter_bboost_en{false};
};

#pragma pack(pop)
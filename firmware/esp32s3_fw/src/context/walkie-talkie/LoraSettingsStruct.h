#pragma once
#include <stdint.h>

#pragma pack(push, 1)

struct LoraSettings
{
    uint8_t aes_key[32];
    uint8_t channel{23};
    uint8_t tr_power{0};
    bool encrypt_en{false};
};

#pragma pack(pop)
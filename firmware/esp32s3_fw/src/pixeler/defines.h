#pragma once

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef ESP32
#include <Arduino.h>
#else
#include <chrono>
#include <functional>

#include "String/WString.h"

#define log_e(fmt, ...) printf("E: " fmt "\n", ##__VA_ARGS__)
#define log_i(fmt, ...) printf("I: " fmt "\n", ##__VA_ARGS__)
#define esp_restart() exit(1)

#define millis() (std::chrono::duration_cast<std::chrono::milliseconds>(   \
                      std::chrono::steady_clock::now().time_since_epoch()) \
                      .count())

#endif  // #ifdef ESP32

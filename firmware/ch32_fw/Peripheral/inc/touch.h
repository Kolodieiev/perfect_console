#ifndef __TOUCH_H
#define __TOUCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <debug.h>

/**
 * @brief Ініціалізує ADC1-модуль та вмикає сенсорний датчик.
 *
 */
void enableTouchCap (void);

/**
 * @brief Вимикає ADC1-модуль ти вимикає сенсорний датчик.
 *
 */
void disableTouchCap (void);

/**
 * @brief Повертає сире значення заряду каналу.
 *
 * @param chann Номер каналу, з якого потрібно прочитати значення заряду.
 * @return uint16_t
 */
uint16_t getTouchRawValue (uint8_t chann);

/**
 * @brief Повертає стан модуля ADC1.
 *
 * @return uint8_t - 0 - Якщо ADC1-модуль вимкнено. 1 - Якщо модуль увімкнено.
 */
uint8_t isAdc1Enabled (void);

#ifdef __cplusplus
}
#endif

#endif /* __TOUCH_H */

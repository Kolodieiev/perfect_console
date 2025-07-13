#ifndef __INPUT_H
#define __INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <debug.h>

/**
 * @brief Вмикає та ініціалізує систему обробки вводу.
 *
 * @param btns_num Максимальна кількість кнопок, які можуть бути задіяні для вводу.
 */
void enableInput (uint8_t btns_num);

/**
 * @brief Вимикає систему вводу та звільняє зайняті нею ресурси.
 *
 */
void disableInput (void);

/**
 * @brief Вмикає кнопку, якщо її було зареєстровано за вказаним віртуальним піном.
 *
 * @param gpio_pin - Віртуальний номер піна, до якого прив'язано кнопку.
 */
void enableBtn (uint16_t gpio_pin);

/**
 * @brief Вимикає кнопку, якщо її було зареєстровано за вказаним віртуальним піном.
 *
 * @param gpio_pin  - Віртуальний номер піна, до якого прив'язано кнопку.
 */
void disableBtn (uint16_t gpio_pin);

/**
 * @brief Встановлює поріг спрацювування сенсорних кнопок.
 *
 * @param treshold
 */
void setInputTouchTreshold (uint16_t treshold);

/**
 * @brief Реєструє нову тактильну кнопку в системі обробки вводу.
 *
 * @param gpio_pin - Віртуальний номер піна.
 */
void addInputBtn (uint16_t gpio_pin);

/**
 * @brief Реєструє нову сенсорну кнопку в системі обробки вводу,
 * якщо для вказаного піна існує ADC-канал.
 *
 * @param gpio_pin Номер піна.
 */
void addInputTouchBtn (uint16_t gpio_pin);

/**
 * @brief Оновлює стан усіх зареєстрованих кнопок.
 *
 */
void inputUpdate (void);

/**
 * @brief Перетворює стан кнопок у бітовий масив.
 * Де кожен біт символізує кнопку в порядку реєстрації її в системі.
 *
 * @param out_buf Вказівник на вихідний буфер, куди буде записано бітовий масив.
 */
void loadInputState (uint8_t *out_buf);

/**
 * @brief Повертає стан утримання кнопки.
 *
 * @param gpio_pin  - Віртуальний номер піна, до якого прив'язано кнопку.
 * @return uint8_t - 1 - якщо кнопка існує і є натиснутою. 0 - в будь-якому іншому випадку.
 */
uint8_t isInputBtnHolded (uint16_t gpio_pin);

#ifdef __cplusplus
}
#endif

#endif /* __INPUT_H */
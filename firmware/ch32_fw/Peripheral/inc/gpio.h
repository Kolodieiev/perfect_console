#ifndef __GPIO_H
#define __GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <debug.h>

typedef enum {
    LOW = 0,
    HIGH = 1
} PinState;

#define INVALID_MAPPING 0xFF
#define ADC_CHANN_NONE 0xFF

/**
 * @brief Налаштовує групу пінів на вказаному порту.
 *
 * @param port - Вказівник на порт, на якому потрібно налаштувати піни. GPIOA/GPIOB і тд.
 * @param pins - Номери пінів, які потрібно налаштувати. GPIO_Pin_0 | GPIO_Pin_1 і тд.
 * @param mode - Режим, на який будуть налаштовані піни. GPIO_Mode_...
 */
void pinModeFast (GPIO_TypeDef *port, uint16_t pins, GPIOMode_TypeDef mode);

/**
 * @brief Налаштовує один віртуальний пін на вказаний режим роботи.
 *
 * @param gpio_pin - Номер віртуального піна.
 * @param mode - Режим роботи піна, за яким закріплено вказаний віртульний пін.
 */
void pinMode (uint8_t gpio_pin, GPIOMode_TypeDef mode);

/**
 * @brief Встановлює логічний рівень піна, який налаштовано в режимі push-pull.
 *
 * @param gpio_pin - Номер віртуального піна.
 * @param state - Логічний рівень, який буде встановлено на піні, за яким закріплено вказаний віртуальний пін.
 */
void digitalWrite (uint8_t gpio_pin, PinState state);

/**
 * @brief Повертає логічний рівень піна, який налаштовано в режимі input або pu/pd.
 *
 * @param gpio_pin - Номер віртуального піна.
 * @return PinState - HIGH якщо на пін подано активний логічний рівень. LOW - у будь-якому іншому випадку.
 */
PinState digitalRead (uint8_t gpio_pin);

/**
 * @brief Повертає номер піна на його порту, до якого закріплено вказаний номер віртуального піна.
 *
 * @param gpio_pin - Номер віртуального піна.
 * @return uint16_t - Номер піна на його порту, якщо такий GPIO-пін існує. INVALID_MAPPING у будь-якому іншому випадку.
 */
uint16_t pinToDigitalPin (uint8_t gpio_pin);

/**
 * @brief Повертає вказівник на порт, на якому знаходиться пін, до якого закріплено вказаний номер віртуального піна.
 *
 * @param gpio_pin - Номер віртуального піна.
 * @return GPIO_TypeDef* - Вказівник на порт, якщо такий GPIO-пін існує. NULL - у будь-якому іншому випадку.
 */
GPIO_TypeDef *pinToPort (uint8_t gpio_pin);

/**
 * @brief Повертає номер ADC-каналу, який закріплено за піном, до якого закріплено вказаний номер віртуального піна.
 *
 * @param gpio_pin - Номер віртуального піна.
 * @return uint16_t - Номер каналу АЦП, якщо за вказаним віртуальним піном закріплено такий канал. ADC_CHANN_NONE - у будь-якому іншому випадку.
 */
uint16_t pinToAdcChann (uint8_t gpio_pin);

/**
 * @brief  Перемикає поточний стан віртуального піна на протилежний, якщо такий GPIO-пін існує.
 *
 * @param gpio_pin - Номер віртуального піна.
 */
void togglePin (uint8_t gpio_pin);

/**
 * @brief Вимикає периферію порта.
 *
 * @param port - Вказівник на порт, який повинен бути вимкнутим. GPIOA/GPIOB і тд.
 */
void disablePort (GPIO_TypeDef *port);

/**
 * @brief Повертає поточний стан порту.
 *
 * @param port - Вказівник на порт, стан якого необхідно перевірити. GPIOA/GPIOB і тд.
 * @return uint8_t - 1 якщо порт увімкнутий. 0 - інакше.
 */
uint8_t isPortEnabled (GPIO_TypeDef *port);


#ifdef __cplusplus
}
#endif

#endif /* __GPIO_H */
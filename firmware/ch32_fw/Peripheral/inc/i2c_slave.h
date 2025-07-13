#ifndef __I2C_H
#define __I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <debug.h>
#include <string.h>

#define I2C_MAX_BUFF_SIZE 40

typedef void (*i2c_read_handler_t) (const uint8_t *in_buff, uint8_t in_buff_size);
typedef void (*i2c_write_handler_t) (uint8_t *out_buff, volatile uint8_t *out_buff_size);

void I2C1_ER_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));
void I2C1_EV_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));

/**
 * @brief Вмикає та налаштовує шину I2C в режимі веденого пристрою.
 *
 * @param freq Частота шини.
 * @param address Адреса веденого пристрою.
 * @param read_cb Колбек читання даних від ведучого пристрою.
 * @param write_cb Колбек надсилання даних до ведучого пристрою.
 * @return uint8_t Результат ініціалізації шини. 1 - якщо шина успішно увімкнена та налаштована. 0 - інакше.
 */
uint8_t enableI2CSlave (uint32_t freq, uint16_t address, i2c_read_handler_t read_cb, i2c_write_handler_t write_cb);

/**
 * @brief Вимикає та деініціалізує шину I2C, що була налаштована в режимі веденого пристрою.
 *
 */
void disableI2CSlave (void);

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H */
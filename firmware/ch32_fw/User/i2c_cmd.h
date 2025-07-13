#ifndef __I2C_CMD_H
#define __I2C_CMD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <debug.h>

typedef enum {
    I2C_CMD_ENABLE = 0,
    I2C_CMD_DISABLE,
    I2C_CMD_PIN_ON,
    I2C_CMD_PIN_OFF,
    I2C_CMD_BTNS_STATE,
    I2C_CMD_OFF_BTN,
    I2C_CMD_ON_BTN,
} ExtI2CMD_t;


#ifdef __cplusplus
}
#endif

#endif /* __I2C_CMD_H */
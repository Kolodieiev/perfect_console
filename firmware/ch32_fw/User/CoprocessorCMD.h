#ifndef __COPROCESSOR_CMD_H
#define __COPROCESSOR_CMD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <debug.h>

typedef enum {
    CCPU_CMD_ENABLE = 0,
    CCPU_CMD_DISABLE,
    CCPU_CMD_PIN_ON,
    CCPU_CMD_PIN_OFF,
    CCPU_CMD_GET_BTNS_STATE,
    CCPU_CMD_OFF_BTN,
    CCPU_CMD_ON_BTN,
} CoprocessorCMD_t;


#ifdef __cplusplus
}
#endif

#endif /* __COPROCESSOR_CMD_H */
#include <debug.h>
#include <gpio.h>
#include <i2c_slave.h>
#include "input/input.h"
#include "i2c_cmd.h"

//-------------------------------------------------------------------------------------------------------------

#define BTNS_NUM 6
#define BTNS_NUM_BUFF_SIZE ((BTNS_NUM + 7) / 8)
//
#define I2C_FREQ 100000
#define RX_ADDR 0x66
#define I2C_TEMP_BUFF_SIZE 2

//-------------------------------------------------------------------------------------------------------------

volatile uint8_t _data_size_to_send = 0;
uint8_t _i2c_out_buff[I2C_TEMP_BUFF_SIZE] = {0};
//
uint8_t _btns_state_buff[BTNS_NUM_BUFF_SIZE] = {0};

//-------------------------------------------------------------------------------------------------------------

void i2c_read_handler (const uint8_t *in_buff, uint8_t in_buff_size) {
    ExtI2CMD_t cmd = in_buff[0];
    switch (cmd) {
    case I2C_CMD_BTNS_STATE:
        memcpy (_i2c_out_buff, _btns_state_buff, BTNS_NUM_BUFF_SIZE);
        _data_size_to_send = BTNS_NUM_BUFF_SIZE;
        break;
    case I2C_CMD_PIN_ON:
        break;
    case I2C_CMD_PIN_OFF:
        break;
    case I2C_CMD_OFF_BTN:
        break;
    case I2C_CMD_ON_BTN:
        break;
    case I2C_CMD_DISABLE:
        break;
    case I2C_CMD_ENABLE:
    default:
        return;
    }
}

void i2c_write_handler (uint8_t *out_buff, volatile uint8_t *out_buff_size) {
    *out_buff_size = _data_size_to_send;
    memcpy (out_buff, _i2c_out_buff, _data_size_to_send);
    for (uint32_t i = 0; i < _data_size_to_send; ++i)
        _i2c_out_buff[i] = 0;

    _data_size_to_send = 0;
}

int main (void) {
    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    initMillis();
    initUsartPrintf (230400);
    //-------------------------------------------------------------------------------------------------------------

    // disablePort (GPIOA);
    // disablePort (GPIOB);
    // disablePort (GPIOC);
    // disablePort (GPIOD); // Не варто, якщо потрібне налагоджування.

    enableI2CSlave (I2C_FREQ, RX_ADDR, i2c_read_handler, i2c_write_handler);

    while (1) {
        inputUpdate();
        loadInputState (_btns_state_buff);

     delay (15);
    }
}

#include "i2c_slave.h"
#include <stdlib.h>

#define I2C_STOP_BYTE 0xFF

static uint8_t *_out_buf = NULL;
static uint8_t _temp_out_counter = 0;
static volatile uint8_t _data_out_counter = 0;

static uint8_t *_in_buf = NULL;
static volatile uint8_t _data_in_counter = 0;

static i2c_read_handler_t _read_handler = NULL;
static i2c_write_handler_t _write_handler = NULL;

void I2C1_ER_IRQHandler (void) {
    if (I2C1->STAR1 & I2C_STAR1_BERR)  // Bus error
        I2C1->STAR1 &= ~(I2C_STAR1_BERR);

    if (I2C1->STAR1 & I2C_STAR1_ARLO)  // Arbitration lost error
        I2C1->STAR1 &= ~(I2C_STAR1_ARLO);

    if (I2C1->STAR1 & I2C_STAR1_AF)  // Acknowledge failure
        I2C1->STAR1 &= ~(I2C_STAR1_AF);
}

uint8_t enableI2CSlave (uint32_t freq, uint16_t address, i2c_read_handler_t read_cb, i2c_write_handler_t write_cb) {

    if (freq == 0 || freq > 100000 || address == 0 || !read_cb || !write_cb) {
        printf ("\nenableI2CSlave: Некоректні аргументи");
        return 0;
    }

    _read_handler = read_cb;
    _write_handler = write_cb;

    if (_out_buf) {
        free (_out_buf);
        _out_buf = NULL;
    }

    _out_buf = (uint8_t *)malloc (I2C_MAX_BUFF_SIZE * sizeof (uint8_t));

    if (!_out_buf) {
        printf ("\nenableI2CSlave: Помилка виділення памя'ті для out_buf");
        return 0;
    }

    if (_in_buf) {
        free (_in_buf);
        _in_buf = NULL;
    }

    _in_buf = (uint8_t *)malloc (I2C_MAX_BUFF_SIZE * sizeof (uint8_t));

    if (!_in_buf) {
        printf ("\nenableI2CSlave: Помилка виділення памя'ті для in_buf");
        free (_out_buf);
        _out_buf = NULL;
        return 0;
    }


    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOC | RCC_PB2Periph_AFIO, ENABLE);
    RCC_PB1PeriphClockCmd (RCC_PB1Periph_I2C1, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init (GPIOC, &GPIO_InitStructure);

    I2C_InitTypeDef I2C_InitTSturcture = {0};
    I2C_InitTSturcture.I2C_ClockSpeed = freq;
    I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
    I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitTSturcture.I2C_OwnAddress1 = address;
    I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
    I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init (I2C1, &I2C_InitTSturcture);
    I2C_Cmd (I2C1, ENABLE);

    I2C1->CTLR2 |= I2C_CTLR2_ITBUFEN | I2C_CTLR2_ITEVTEN | I2C_CTLR2_ITERREN;

    NVIC_InitTypeDef NVIC_InitStructure = {0};

    NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);

    return 1;
}

void disableI2CSlave (void) {
    NVIC_DisableIRQ (I2C1_EV_IRQn);
    NVIC_DisableIRQ (I2C1_ER_IRQn);

    I2C_Cmd (I2C1, DISABLE);
    I2C_DeInit (I2C1);
    I2C_AcknowledgeConfig (I2C1, DISABLE);

    RCC_PB2PeriphClockCmd (RCC_PB2Periph_AFIO, DISABLE);
    RCC_PB1PeriphClockCmd (RCC_PB1Periph_I2C1, DISABLE);


    if (_out_buf) {
        free (_out_buf);
        _out_buf = NULL;
    }

    if (_in_buf) {
        free (_in_buf);
        _in_buf = NULL;
    }
}

void I2C1_EV_IRQHandler (void) {

    uint16_t sr1 = I2C1->STAR1;

    if (sr1 & I2C_STAR1_SB) {
        (void)I2C1->STAR1;
        (void)I2C1->STAR2;
        return;
    }

    if (sr1 & I2C_STAR1_ADDR) {
        (void)I2C1->STAR1;
        (void)I2C1->STAR2;
        _data_in_counter = 0;
        _temp_out_counter = 0;
        _data_out_counter = 0;
        return;
    }

    if (sr1 & I2C_STAR1_RXNE) {
        u8 data = I2C_ReceiveData (I2C1);

        if (_data_in_counter < I2C_MAX_BUFF_SIZE)
            _in_buf[_data_in_counter++] = data;

        return;
    }

    if (sr1 & I2C_STAR1_STOPF) {
        (void)I2C1->STAR1;
        I2C1->CTLR1 |= I2C_CTLR1_PE;

        if (_data_in_counter > 0)
            _read_handler (_in_buf, _data_in_counter);

        return;
    }

    if (sr1 & I2C_STAR1_TXE) {
        if (_data_out_counter == 0)
            _write_handler (_out_buf, &_data_out_counter);

        if (_temp_out_counter == _data_out_counter) {
            I2C_SendData (I2C1, I2C_STOP_BYTE);
        } else {
            I2C_SendData (I2C1, _out_buf[_temp_out_counter]);
            ++_temp_out_counter;
        }
        return;
    }

    printf ("\nUnknown i2c event SR1=0x%04X SR2=0x%04X\n", I2C1->STAR1, I2C1->STAR2);
}

#include <debug.h>
#include <gpio.h>
#include <i2c_slave.h>
#include "input.h"
#include "touch.h"
#include "CoprocessorCMD.h"

// #define DEBUG_LOG

//------------------------------------------------------------------------------------------------------------- btns
#define BTNS_NUM 7

#define PIN_POWER_BTN 23

#define PIN_PTT_BTN 22
#define PIN_OK_BTN 19
#define PIN_BACK_BTN 8
#define PIN_LEFT_BTN 11
#define PIN_RIGHT_BTN 17
#define PIN_UP_BTN 9
#define PIN_DOWN_BTN 10

#define PIN_PB1 20
#define PIN_PB3 21

#define INPUT_UPD_TIME_MS 10

//------------------------------------------------------------------------------------------------------------- keys
#define PIN_MIC_KEY 14
#define PIN_DISPLAY_KEY 15
#define PIN_POWER_KEY 16
#define PIN_LORA_KEY 18
#define PIN_AUDIO_OUT_KEY 24

#define PIN_I2C_INT 2

//------------------------------------------------------------------------------------------------------------- i2c

#define BTNS_NUM_BUFF_SIZE ((BTNS_NUM + 7) / 8)
#define I2C_TEMP_BUFF_SIZE 5

#define I2C_FREQ 100000
#define RX_ADDR 0x66

//

volatile uint8_t _data_size_to_send = 0;
uint8_t _i2c_out_buff[I2C_TEMP_BUFF_SIZE] = {0};
uint8_t _btns_state_buff[BTNS_NUM_BUFF_SIZE] = {0};

//------------------------------------------------------------------------------------------------------------- power
#define TIME_TO_TOGGLE_PWR 3000
uint8_t _is_sleeping = 0;

//------------------------------------------------------------------------------------------------------------- func
void initPins (void);
void deinitPins (void);

void initPwrBtnExti (void);
void deinitPwrBtnExti (void);

void goToSleep (void);
void wakeUp (void);
void EXTI7_0_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));

void i2c_read_handler (const uint8_t *in_buff, uint8_t in_buff_size);
void i2c_write_handler (uint8_t *out_buff, volatile uint8_t *out_buff_size);

int main (void) {
    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    initMillis();
    initUsartPrintf (230400);

    //-----------------------

#ifdef DEBUG_LOG
    printf ("Begin...\r\n");
#endif

    pinMode (PIN_POWER_BTN, GPIO_Mode_IPU);
    initPins();
    enableI2CSlave (I2C_FREQ, RX_ADDR, i2c_read_handler, i2c_write_handler);
    goToSleep();

    unsigned long power_btn_press_time = 0;

    while (1) {
        if (_is_sleeping == 1) {
            if (digitalRead (PIN_POWER_BTN) == HIGH) {
                power_btn_press_time = 0;
#ifdef DEBUG_LOG
                printf ("PIN_POWER_BTN == HIGH\r\n");
#endif
                goToSleep();
            } else {
                if (power_btn_press_time == 0) {
                    power_btn_press_time = millis();
                } else if (millis() - power_btn_press_time > TIME_TO_TOGGLE_PWR) {
                    power_btn_press_time = 0;
                    wakeUp();
                }
            }

            continue;
        }

        if (digitalRead (PIN_POWER_BTN) == LOW) {
            if (power_btn_press_time == 0) {
                power_btn_press_time = millis();
            } else if (millis() - power_btn_press_time > TIME_TO_TOGGLE_PWR) {
                power_btn_press_time = 0;
                goToSleep();
            }

            continue;
        } else {
            power_btn_press_time = 0;
        }

        static unsigned long input_update_time = 0;
        if (millis() - input_update_time > INPUT_UPD_TIME_MS) {
            inputUpdate();
            loadInputState (_btns_state_buff);
            input_update_time = millis();
        }
    }
}

void initPins (void) {
#ifdef DEBUG_LOG
    printf ("initPins\r\n");
#endif

    pinMode (PIN_POWER_KEY, GPIO_Mode_Out_PP);
    digitalWrite (PIN_POWER_KEY, HIGH);

    pinMode (PIN_DISPLAY_KEY, GPIO_Mode_Out_PP);
    digitalWrite (PIN_DISPLAY_KEY, LOW);

    pinMode (PIN_AUDIO_OUT_KEY, GPIO_Mode_Out_PP);
    digitalWrite (PIN_AUDIO_OUT_KEY, LOW);

    pinMode (PIN_MIC_KEY, GPIO_Mode_Out_PP);
    digitalWrite (PIN_MIC_KEY, LOW);

    pinMode (PIN_LORA_KEY, GPIO_Mode_Out_PP);
    digitalWrite (PIN_LORA_KEY, LOW);

    enableInput (BTNS_NUM);

    addInputBtn (PIN_PTT_BTN);
    addInputBtn (PIN_OK_BTN);
    addInputBtn (PIN_BACK_BTN);
    addInputBtn (PIN_LEFT_BTN);
    addInputBtn (PIN_RIGHT_BTN);
    addInputBtn (PIN_UP_BTN);
    addInputBtn (PIN_DOWN_BTN);

    // addInputTouchBtn (PIN_OK_BTN);
    // addInputTouchBtn (PIN_BACK_BTN);
    // addInputTouchBtn (PIN_LEFT_BTN);
    // addInputTouchBtn (PIN_RIGHT_BTN);
    // addInputTouchBtn (PIN_UP_BTN);
    // addInputTouchBtn (PIN_DOWN_BTN);
}

void deinitPins (void) {
#ifdef DEBUG_LOG
    printf ("deinitPins\r\n");
#endif

    disableInput();
    disableTouchCap();

    pinMode (PIN_MIC_KEY, GPIO_Mode_IN_FLOATING);
    pinMode (PIN_DISPLAY_KEY, GPIO_Mode_IN_FLOATING);
    pinMode (PIN_POWER_KEY, GPIO_Mode_IN_FLOATING);
    pinMode (PIN_LORA_KEY, GPIO_Mode_IN_FLOATING);
    pinMode (PIN_AUDIO_OUT_KEY, GPIO_Mode_IN_FLOATING);
    pinMode (PIN_PTT_BTN, GPIO_Mode_IN_FLOATING);
    pinMode (PIN_OK_BTN, GPIO_Mode_IN_FLOATING);
    pinMode (PIN_BACK_BTN, GPIO_Mode_IN_FLOATING);
    pinMode (PIN_LEFT_BTN, GPIO_Mode_IN_FLOATING);
    pinMode (PIN_RIGHT_BTN, GPIO_Mode_IN_FLOATING);
    pinMode (PIN_UP_BTN, GPIO_Mode_IN_FLOATING);
    pinMode (PIN_DOWN_BTN, GPIO_Mode_IN_FLOATING);
}

void initPwrBtnExti (void) {
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_AFIO, ENABLE);

    EXTI_InitTypeDef EXTI_InitStructure = {0};
    GPIO_EXTILineConfig (GPIO_PortSourceGPIOB, GPIO_PinSource1);
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init (&EXTI_InitStructure);

    NVIC_InitTypeDef NVIC_InitStructure = {0};
    NVIC_InitStructure.NVIC_IRQChannel = EXTI7_0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);
}

void deinitPwrBtnExti (void) {
    EXTI_DeInit();
    NVIC_DisableIRQ (EXTI7_0_IRQn);
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_AFIO, DISABLE);
}

void goToSleep (void) {
#ifdef DEBUG_LOG
    printf ("goToSleep\r\n");
#endif

    _is_sleeping = 1;
    initPwrBtnExti();
    deinitPins();
    PWR_EnterSTANDBYMode (PWR_STANDBYEntry_WFE);
}

void wakeUp (void) {
#ifdef DEBUG_LOG
    printf ("wakeUp\r\n");
#endif
    deinitPwrBtnExti();
    _is_sleeping = 0;
    initPins();
}

void EXTI7_0_IRQHandler (void) {
    if (EXTI_GetITStatus (EXTI_Line1) != RESET) {
#ifdef DEBUG_LOG
        printf ("EXTI\r\n");
#endif
        EXTI_ClearITPendingBit (EXTI_Line1);
    }
}

void i2c_read_handler (const uint8_t *in_buff, uint8_t in_buff_size) {
    CoprocessorCMD_t cmd = in_buff[0];
    switch (cmd) {
    case CCPU_CMD_GET_BTNS_STATE:
        memcpy (_i2c_out_buff, _btns_state_buff, BTNS_NUM_BUFF_SIZE);
        _data_size_to_send = BTNS_NUM_BUFF_SIZE;
        break;
    case CCPU_CMD_PIN_ON:
        if (in_buff_size < 2)
            return;
        digitalWrite (in_buff[1], HIGH);
        break;
    case CCPU_CMD_PIN_OFF:
        if (in_buff_size < 2)
            return;
        digitalWrite (in_buff[1], LOW);
        break;
    case CCPU_CMD_BTN_OFF:
        if (in_buff_size < 2)
            return;
        disableBtnByPos (in_buff[1]);
        break;
    case CCPU_CMD_BTN_ON:
        if (in_buff_size < 2)
            return;
        enableBtnByPos (in_buff[1]);
        break;
    case CCPU_CMD_DISABLE:
    case CCPU_CMD_ENABLE:
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
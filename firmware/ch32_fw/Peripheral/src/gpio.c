#include "gpio.h"

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
    uint8_t adc_chann;
} PinMapping;

// CH32V006E8R6
const PinMapping _pin_map[] = {
    {GPIOD, GPIO_Pin_1,      ADC_CHANN_NONE}, // 1
    {GPIOC, GPIO_Pin_0,      ADC_CHANN_NONE}, // 2
    {GPIOC, GPIO_Pin_1,      ADC_CHANN_NONE}, // 3
    {GPIOC, GPIO_Pin_2,      ADC_CHANN_NONE}, // 4
    {NULL,  INVALID_MAPPING, ADC_CHANN_NONE}, // 5 GND
    {NULL,  INVALID_MAPPING, ADC_CHANN_NONE}, // 6 VCC
    {GPIOC, GPIO_Pin_5,      ADC_CHANN_NONE}, // 7
    {GPIOC, GPIO_Pin_4,      2             }, // 8
    {GPIOD, GPIO_Pin_2,      3             }, // 9
    {GPIOD, GPIO_Pin_3,      4             }, // 10
    {GPIOD, GPIO_Pin_4,      7             }, // 11
    {GPIOD, GPIO_Pin_5,      5             }, // 12
    {GPIOD, GPIO_Pin_6,      6             }, // 13
    {GPIOD, GPIO_Pin_7,      ADC_CHANN_NONE}, // 14
    {GPIOA, GPIO_Pin_4,      ADC_CHANN_NONE}, // 15
    {GPIOA, GPIO_Pin_5,      ADC_CHANN_NONE}, // 16
    {GPIOA, GPIO_Pin_1,      1             }, // 17
    {GPIOA, GPIO_Pin_0,      ADC_CHANN_NONE}, // 18
    {GPIOA, GPIO_Pin_2,      0             }, // 19
    {GPIOD, GPIO_Pin_0,      ADC_CHANN_NONE}, // 20
    {GPIOA, GPIO_Pin_3,      ADC_CHANN_NONE}, // 21
    {GPIOB, GPIO_Pin_0,      ADC_CHANN_NONE}, // 22
    {GPIOB, GPIO_Pin_1,      ADC_CHANN_NONE}, // 23
    {GPIOB, GPIO_Pin_3,      ADC_CHANN_NONE}, // 24
};

uint32_t portToMask (GPIO_TypeDef *port) {
    if (port == GPIOA)
        return RCC_PB2Periph_GPIOA;
    if (port == GPIOB)
        return RCC_PB2Periph_GPIOB;
    if (port == GPIOC)
        return RCC_PB2Periph_GPIOC;
    if (port == GPIOD)
        return RCC_PB2Periph_GPIOD;
    else {
        printf ("\nError: Unknown gpio port");
        return 0;
    }
}

void disablePort (GPIO_TypeDef *port) {
    if (!isPortEnabled (port))
        return;

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit (&GPIO_InitStructure);

    GPIO_Init (port, &GPIO_InitStructure);
    uint32_t mask = portToMask (port);
    if (mask == 0)
        return;

    RCC_PB2PeriphClockCmd (mask, DISABLE);
}

uint8_t isPortEnabled (GPIO_TypeDef *port) {
    uint32_t mask = portToMask (port);
    if (mask == 0)
        return DISABLE;

    if ((RCC->PB2PCENR & mask) > 0)
        return ENABLE;

    return DISABLE;
}

void pinModeFast (GPIO_TypeDef *port, uint16_t pins, GPIOMode_TypeDef mode) {
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Pin = pins,
        .GPIO_Speed = GPIO_Speed_30MHz,
        .GPIO_Mode = mode};

    uint32_t mask = portToMask (port);
    if (mask == 0)
        return;

    RCC_PB2PeriphClockCmd (mask, ENABLE);
    GPIO_Init (port, &GPIO_InitStructure);
}

uint16_t pinToDigitalPin (uint8_t gpio_pin) {
    if (gpio_pin == 0 || gpio_pin > sizeof (_pin_map)) {
        printf ("\nError: Некоректний номер піна: %u", gpio_pin);
        return INVALID_MAPPING;
    }

    u16 pin = _pin_map[gpio_pin - 1].pin;

    if (pin == INVALID_MAPPING)
        printf ("\nError: Не GPIO пін: %u", gpio_pin);

    return pin;
}

GPIO_TypeDef *pinToPort (uint8_t gpio_pin) {
    if (gpio_pin == 0 || gpio_pin > sizeof (_pin_map)) {
        printf ("\nError: Некоректний номер піна: %u", gpio_pin);
        return NULL;
    }

    GPIO_TypeDef *port = _pin_map[gpio_pin - 1].port;

    if (port == NULL)
        printf ("\nError: Не GPIO пін: %u", gpio_pin);

    return port;
}

uint16_t pinToAdcChann (uint8_t gpio_pin) {
    if (gpio_pin == 0 || gpio_pin > sizeof (_pin_map)) {
        printf ("\nError: Некоректний номер піна: %u", gpio_pin);
        return ADC_CHANN_NONE;
    }

    u8 adc_chann = _pin_map[gpio_pin - 1].adc_chann;

    if (adc_chann == ADC_CHANN_NONE)
        printf ("\nError: Пін не має ADC-каналу: %u", gpio_pin);
    
    return adc_chann;
}

void pinMode (uint8_t gpio_pin, GPIOMode_TypeDef mode) {
    if (mode == GPIO_Mode_AIN) {
        u8 adc_chann = pinToAdcChann (gpio_pin);

        if (adc_chann == ADC_CHANN_NONE)
            return;
    }

    u16 pin = pinToDigitalPin (gpio_pin);
    if (pin == INVALID_MAPPING)
        return;

    pinModeFast (_pin_map[gpio_pin - 1].port, pin, mode);
}

void digitalWrite (uint8_t gpio_pin, PinState state) {
    u16 pin = pinToDigitalPin (gpio_pin);
    if (pin == INVALID_MAPPING)
        return;

    GPIO_WriteBit (_pin_map[gpio_pin - 1].port, pin, (BitAction)state);
}

PinState digitalRead (uint8_t gpio_pin) {
    u16 pin = pinToDigitalPin (gpio_pin);
    if (pin == INVALID_MAPPING)
        return LOW;

    if (GPIO_ReadInputDataBit (_pin_map[gpio_pin - 1].port, pin) == Bit_RESET)
        return LOW;

    return HIGH;
}

void togglePin (uint8_t gpio_pin) {
    u16 pin = pinToDigitalPin (gpio_pin);
    if (pin == INVALID_MAPPING)
        return;

    u8 val = GPIO_ReadOutputDataBit (_pin_map[gpio_pin - 1].port, pin);
    val = !val;

    GPIO_WriteBit (_pin_map[gpio_pin - 1].port, pin, val);
}

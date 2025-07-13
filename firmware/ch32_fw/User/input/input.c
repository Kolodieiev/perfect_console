#include "input.h"
#include <stdlib.h>
#include <touch.h>
#include <gpio.h>

#define DEFAULT_TOUCH_TRESHOLD 3950

typedef struct Button {
    GPIO_TypeDef *port;
    uint16_t pin;
    uint8_t adc_chann;
    uint8_t is_touch_pin;
    uint8_t is_holded;
    uint8_t is_enabled;
} Button;

typedef struct Input {
    Button *butons;
    uint16_t touch_treshold;
    uint8_t btns_num;
} Input;

static Input *_input = NULL;
static uint8_t _btns_counter = 0;

void enableInput (uint8_t btns_num) {
    if (btns_num == 0)
        return;

    disableInput();

    _input = (Input *)malloc (sizeof (Input));

    if (!_input) {
        printf ("Помилка виділення памя'ті для _input");
        return;
    }

    _input->butons = (Button *)malloc (sizeof (Button) * btns_num);

    if (!_input->butons) {
        printf ("Помилка виділення памя'ті для _input->butons");
        disableInput();
        return;
    }

    _input->touch_treshold = DEFAULT_TOUCH_TRESHOLD;
    _input->btns_num = btns_num;
}

void disableInput (void) {
    if (_input) {
        if (_input->butons) {
            free (_input->butons);
            _input->butons = NULL;
        }

        free (_input);
        _input = NULL;
    }

    _btns_counter = 0;
}

Button *findRegBtn (uint16_t gpio_pin) {
    GPIO_TypeDef *port = pinToPort (gpio_pin);

    if (port) {
        u8 pin = pinToDigitalPin (gpio_pin);

        for (uint32_t i = 0; i < _btns_counter; ++i) {
            if (_input->butons[i].port == port && _input->butons[i].pin == pin)
                return &_input->butons[i];
        }
    }

    return NULL;
}

void enableBtn (uint16_t gpio_pin) {
    if (!_input) {
        printf ("\nError: input не було ініціалізовано");
        return;
    }

    Button *btn = findRegBtn (gpio_pin);

    if (!btn)
        return;

    btn->is_enabled = ENABLE;
}

void disableBtn (uint16_t gpio_pin) {
    if (!_input) {
        printf ("\nError: input не було ініціалізовано");
        return;
    }

    Button *btn = findRegBtn (gpio_pin);

    if (!btn)
        return;

    btn->is_enabled = DISABLE;
}

void setInputTouchTreshold (uint16_t treshold) {
    _input->touch_treshold = treshold;
}

void addInputBtn (uint16_t gpio_pin) {
    if (!_input) {
        printf ("\nError: input не було ініціалізовано");
        return;
    }

    if (_btns_counter >= _input->btns_num)
        return;

    if (findRegBtn (gpio_pin) != NULL)
        return;

    u16 pin = pinToDigitalPin (gpio_pin);
    if (pin == INVALID_MAPPING)
        return;

    _input->butons[_btns_counter].pin = pin;
    _input->butons[_btns_counter].port = pinToPort (gpio_pin);
    _input->butons[_btns_counter].is_touch_pin = 0;
    _input->butons[_btns_counter].is_holded = 0;
    _input->butons[_btns_counter].is_enabled = 1;

    pinModeFast (_input->butons[_btns_counter].port, _input->butons[_btns_counter].pin, GPIO_Mode_IPU);

    ++_btns_counter;
}

void addInputTouchBtn (uint16_t gpio_pin) {
    if (!_input) {
        printf ("\nError: input не було ініціалізовано");
        return;
    }

    if (_btns_counter >= _input->btns_num)
        return;

    if (findRegBtn (gpio_pin) != NULL)
        return;

    u8 adc_chann = pinToAdcChann (gpio_pin);
    if (adc_chann == ADC_CHANN_NONE)
        return;

    _input->butons[_btns_counter].pin = pinToDigitalPin (gpio_pin);
    _input->butons[_btns_counter].port = pinToPort (gpio_pin);
    _input->butons[_btns_counter].adc_chann = adc_chann;

    _input->butons[_btns_counter].is_touch_pin = 1;
    _input->butons[_btns_counter].is_holded = 0;
    _input->butons[_btns_counter].is_enabled = 1;

    enableTouchCap();
    pinModeFast (_input->butons[_btns_counter].port, _input->butons[_btns_counter].pin, GPIO_Mode_AIN);

    ++_btns_counter;
}

void inputUpdate (void) {
    if (!_input) {
        printf ("\nError: input не було ініціалізовано");
        return;
    }

    for (uint32_t i = 0; i < _btns_counter; ++i) {
        if (!_input->butons[i].is_enabled) {
            _input->butons[i].is_holded = 0;
            continue;
        }

        if (_input->butons[i].is_touch_pin == 0) {
            u8 val = GPIO_ReadInputDataBit (_input->butons[i].port, _input->butons[i].pin);

            if (val > 0)
                _input->butons[i].is_holded = 0;
            else
                _input->butons[i].is_holded = 1;

        } else {
            u16 raw = getTouchRawValue (_input->butons[i].adc_chann);

            if (raw > _input->touch_treshold)
                _input->butons[i].is_holded = 0;
            else
                _input->butons[i].is_holded = 1;
        }
    }
}

void loadInputState (uint8_t *out_buf) {
    if (!_input) {
        printf ("\nError: input не було ініціалізовано");
        return;
    }

    for (uint32_t i = 0; i < _input->btns_num; ++i) {
        size_t byte_index = i / 8;
        uint8_t bit_mask = 1 << (7 - (i % 8));

        if (_input->butons[i].is_holded)
            out_buf[byte_index] |= bit_mask;
        else
            out_buf[byte_index] &= ~bit_mask;
    }
}

uint8_t isInputBtnHolded (uint16_t gpio_pin) {
    if (!_input) {
        printf ("\nError: input не було ініціалізовано");
        return 0;
    }

    Button *btn = findRegBtn (gpio_pin);

    if (btn)
        return btn->is_holded;

    printf ("\nError: Кнопки за вказаним піном не існує: %u", gpio_pin);
    return 0;
}

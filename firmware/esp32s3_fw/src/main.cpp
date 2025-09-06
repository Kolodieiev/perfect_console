#include <Arduino.h>
#include "meow/Meowui.h"
#include "meowui_setup/input_setup.h"
#include "meow/manager/CoprocessorManager.h"
#include "context/resources/ch32_pins_def.h"

using namespace meow;

void setup()
{
    // Перевірити з'єднання з допоміжним МК.
    if (!meow::_ccpu.connect())
        esp_restart();

    // Увімкнути живлення дисплею з допоміжного МК.
    uint8_t ccpu_cmd_data[2]{CCPU_CMD_PIN_ON, CH_PIN_DISPLAY_PWR};
    _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data));

    // Увімкнути всі кнопки на допоміжному МК.
    ccpu_cmd_data[0] = CCPU_CMD_BTN_ON;

    ccpu_cmd_data[1] = BtnID::BTN_OK;
    _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

    ccpu_cmd_data[1] = BtnID::BTN_BACK;
    _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

    ccpu_cmd_data[1] = BtnID::BTN_LEFT;
    _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

    ccpu_cmd_data[1] = BtnID::BTN_RIGHT;
    _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

    ccpu_cmd_data[1] = BtnID::BTN_UP;
    _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

    ccpu_cmd_data[1] = BtnID::BTN_DOWN;
    _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

    // Запустити виконання Meowui.
    MEOW.begin(80);
}

void loop()
{
    vTaskDelete(NULL);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

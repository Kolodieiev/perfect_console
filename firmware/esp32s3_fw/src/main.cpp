#include <Arduino.h>
#include "meow/manager/context/ContextManager.h"
#include "meow/manager/coprocessor/CoprocessorManager.h"
#include "context/resources/ch32_pins_def.h"
#include "meowui_setup/display_setup.h"
#include "meowui_setup/sd_setup.h"
#include "meowui_setup/input_setup.h"

using namespace meow;

void guiTask(void *params)
{
    MEOW.run();
}

void setup()
{
    if (!meow::_ccpu.connect())
        esp_restart();

    uint8_t ccpu_cmd_data[2]{CCPU_CMD_PIN_ON, CH_PIN_DISPLAY_PWR};
    _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data));

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

    xTaskCreatePinnedToCore(guiTask, "guiTask", (1024 / 2) * 50, NULL, 10, NULL, 1);
}

void loop()
{
    vTaskDelete(NULL);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

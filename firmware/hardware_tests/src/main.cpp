// #define COMMON_TEST      // Загальне тестування: підключення до CH32, підключення кати пам'яті та дисплею.
#define AUDIO_TEST       // Тест мікрофону та звукової карти. Звук зчитується з мікрофону, та транслюється в звукову карту.
#define LORA_TEST_MASTER // Тестування модуля LoRa в режимі надсилання пакетів.
#define LORA_TEST_SLAVE  // Тестування модуля LoRa в режимі прийому пакетів.

#define MP3_FILE_PATH "test.mp3" // Шлях до mp3-файлу на карті пам'яті

//--------------------------------------------------------------------------------------------

#include <Arduino.h>
#include "meowui_setup/sd_setup.h"
#include "meowui_setup/display_setup.h"
#include "meow/manager/i2c/I2C_Manager.h"
#include "meow/manager/spi/SPI_Manager.h"
#include "meow/manager/sd/SD_Manager.h"
#include "meow/manager/files/FileManager.h"
#include "meow/driver/graphics/TFT_eSPI/TFT_eSPI.h"
#include "meow/manager/audio/in/I2SInManager.h"
#include "meow/manager/audio/out/I2SOutManager.h"
//----------------------------------------
using namespace meow;
#define CH32_ADDR 0x66
I2C_Manager _i2c;
SPI_Manager _spi;
//----------------------------------------

void conectToCH32()
{
    delay(200);
    log_i("Підключення до CH32...");

    if (!_i2c.begin())
        esp_restart();

    if (!_i2c.hasConnect(CH32_ADDR))
        esp_restart();

    log_i("Підключення до CH32: OK");
}

void startSPI()
{
    log_i("Запуск SPI...");

    if (!_spi.initBus(HSPI, SD_PIN_SCLK, SD_PIN_MISO, SD_PIN_MOSI))
    {
        log_e("Помилка запуску шини SPI");
        esp_restart();
    }

    log_i("Запуск SPI: OK");
}

#if defined(COMMON_TEST)

void setup()
{
    // ---------------------------------------------------------------------------------------- Карта пам'яті

    conectToCH32();
    startSPI();

    pinMode(SD_PIN_CS, OUTPUT);
    digitalWrite(SD_PIN_CS, HIGH);
    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);

    log_i("Тестування SD-Card...");

    SD_Manager &sd = SD_Manager::getInst();
    sd.setup(SD_PIN_CS, _spi.getSpi4Bus(HSPI), SD_FREQUENCY);

    if (!sd.mount())
        esp_restart();

    FileManager fm;

    log_i("Запис на SD-Card...");
    FILE *f = fm.openFile("/sd_card_test.ext", "w");

    if (!f)
    {
        sd.unmount();
        esp_restart();
    }

    fm.closeFile(f);

    if (!fm.startRemoving("/sd_card_test.ext"))
    {
        sd.unmount();
        esp_restart();
    }

    delay(100);
    while (fm.isWorking())
        delay(1);

    if (!fm.lastTaskResult())
    {
        log_e("Помилка видалення тестового файлу");
        sd.unmount();
        esp_restart();
    }

    log_i("Запис на SD-Card: OK");
    log_i("Тестування SD-Card: OK");

    sd.unmount();

    // ---------------------------------------------------------------------------------------- Дисплей

    log_i("Тестування дисплею...");

    log_i("Вмикаю живлення дисплею та його підсвітку..."); // TODO

    pinMode(BACKLIGHT_PIN, OUTPUT);
    digitalWrite(BACKLIGHT_PIN, HIGH);

    EXT_I2C_CMD cmd = I2C_CMD_PIN_ON;
    _i2c.write(CH32_ADDR, &cmd, sizeof(cmd));

    TFT_eSPI tft;
    tft.setRotation(3);
    tft.init();

    log_i("Дисплей повинен почати змінювати колір заливки");

    tft.fillScreen(TFT_RED);
    delay(1000);
    tft.fillScreen(TFT_GREEN);
    delay(1000);
    tft.fillScreen(TFT_BLUE);
    delay(1000);

    log_i("Вимикаю живлення дисплею");
    log_i("Тестування дисплею: Завершено");
}

void loop()
{
}

#elif defined(AUDIO_TEST)
// ---------------------------------------------------------------------------------------- Читання звуку з мікрофону і надсилання його до звукової карти

#define BUFF_SIZE 200

int16_t buff[BUFF_SIZE];

void setup()
{
    //TODO enable modules
    _i2s_in.init();
    _i2s_out.init();

    _i2s_in.enable();
    _i2s_out.enable();
}

void loop()
{
    size_t samples_read = _i2s_in.read(buff, BUFF_SIZE);
    _i2s_out.write(buff, samples_read, true);
}

#elif defined(LORA_TEST_MASTER)
// ---------------------------------------------------------------------------------------- Надсилання пакетів модулем LoRa
void setup() {}

void loop() {}

#elif defined(LORA_TEST_SLAVE)
// ---------------------------------------------------------------------------------------- Читання пакетів модулем LoRa
void setup() {}

void loop() {}

#endif

#include "CodecSetsContext.h"
#include "meow/manager/settings/SettingsManager.h"
#include "../WidgetCreator.h"

const char STR_AUDIO_TEST[] = "Тест аудіо";

#define SAMPLE_RATE 16000U

CodecSetsContext::CodecSetsContext()
{
    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    loadCodecSettings();

    _codec = codec2_create(CODEC2_MODE_1200);
    _samples_per_frame = codec2_samples_per_frame(_codec);
    _codec_buf_size = codec2_bytes_per_frame(_codec);
    _codec_buf = (uint8_t *)malloc(_codec_buf_size);
    _samples_16k_num = _samples_per_frame * 2;
    _samples_16k_buf = (int16_t *)malloc(_samples_16k_num * sizeof(int16_t));
    _samples_8k_buf = (int16_t *)malloc(_samples_per_frame * sizeof(int16_t));

    codec2_set_lpc_post_filter(
        _codec,
        static_cast<int>(_codec_sets.post_filter_en),
        _codec_sets.post_filter_bass_boost,
        _codec_sets.post_filter_beta,
        _codec_sets.post_filter_gamma); // TODO  Винести в окремий метод для динамічної зміни з GUI.

    showMainTmpl();
}

CodecSetsContext::~CodecSetsContext()
{
    codec2_destroy(_codec);
    free(_codec_buf);
    free(_samples_8k_buf);
    free(_samples_16k_buf);
}

bool CodecSetsContext::loop()
{
    // _i2s_in.read(samples_16k_buf, samples_16k_num);
    // hps.filter(samples_16k_buf, samples_16k_num); // TODO Додати підтримку динамічного редагування фільтрів з GUI.
    // downsampleX2(samples_16k_buf, samples_8k_buf, samples_16k_num);
    // agc.process(samples_8k_buf, samples_per_frame);
    // //
    // codec2_encode_1200(_codec, codec_buf, samples_8k_buf);
    // codec2_decode_1200(_codec, samples_8k_buf, codec_buf);
    // //
    // agc2.process(samples_8k_buf, samples_per_frame);
    // //
    // upsampleX2(samples_8k_buf, samples_16k_buf, samples_per_frame);
    // _i2s_out.write(samples_16k_buf, samples_16k_num, true);

    if (millis() - _last_delay_time > 1000)
    {
        vTaskDelay(1 / portTICK_PERIOD_MS);
        _last_delay_time = millis();
    }

    return true;
}

void CodecSetsContext::update()
{
    if (_input.isPressed(BtnID::BTN_BACK))
    {
        _input.lock(BtnID::BTN_BACK, PRESS_LOCK);
        release();
    }
}

void CodecSetsContext::showMainTmpl()
{
}

void CodecSetsContext::showContextMenuTmpl()
{
    // // Тест аудіо
    // MenuItem *audio_item = creator.getMenuItem(ID_ITEM_AUDIO_TEST);
    // _context_menu->addItem(audio_item);

    // Label *audio_lbl = creator.getItemLabel(STR_AUDIO_TEST, 4);
    // audio_item->setLbl(audio_lbl);
    // audio_lbl->setTextOffset(1);
}

void CodecSetsContext::loadCodecSettings()
{
    String sets_path = SettingsManager::getSettingsFilePath("");
    sets_path += STR_CODEC_SETS_DIR;

    if (!_fs.dirExist(sets_path.c_str(), true))
        return;

    sets_path += STR_CODEC_SETS_NAME;

    if (_fs.fileExist(sets_path.c_str()))
        _fs.readFile(sets_path.c_str(), &_codec_sets, sizeof(_codec_sets));
}

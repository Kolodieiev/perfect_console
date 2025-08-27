#pragma once
#include <Arduino.h>
#include <codec2.h>
#include "meow/ui/context/IContext.h"
#include "meow/lib/audio/filter/filter.h"
#include "meow/manager/audio/in/I2SInManager.h"
#include "meow/manager/audio/out/I2SOutManager.h"
#include "CodecSettingsStruct.h"

using namespace meow;

class CodecSetsContext : public IContext
{
public:
    CodecSetsContext();
    virtual ~CodecSetsContext();

protected:
    virtual bool loop() override;
    virtual void update() override;

private:
    enum Mode : uint8_t
    {
        MODE_MAIN = 0,
        MODE_CTX_MENU,
    };

    enum Widget_ID : uint8_t
    {
        ID_MAIN_MENU = 1,
        ID_CONTEXT_MENU,
        //
        ID_SAVE_SETS_ITEM,
        ID_TEST_AUDIO_ITEM,
    };

    void showMainTmpl();
    void showContextMenuTmpl();
    //
    void loadCodecSettings();

private:
    CodecSettings _codec_sets;

    HighPassFilter _hpf;
    SimpleAGC _agc_out;
    SimpleAGC _agc_in;

    struct CODEC2 *_codec{nullptr};
    uint8_t *_codec_buf{nullptr};

    int16_t *_samples_16k_buf{nullptr};
    int16_t *_samples_8k_buf{nullptr};

    int _samples_per_frame{0};
    int _codec_buf_size{0};
    uint16_t _samples_16k_num{0};

    Mode _mode{MODE_MAIN};

    unsigned long _last_delay_time{0};

    bool _audio_test_enabled{false};
};

#pragma once
#include <Arduino.h>
#include <codec2.h>
#include "meow/ui/context/IContext.h"
#include "meow/lib/audio/filter/filter.h"
#include "meow/manager/audio/in/I2SInManager.h"
#include "meow/manager/audio/out/I2SOutManager.h"
#include "CodecSettingsStruct.h"
#include "meow/ui/widget/text/Label.h"
#include "meow/ui/widget/menu/FixedMenu.h"
#include "meow/ui/widget/toggle/ToggleSwitch.h"
#include "meow/ui/widget/scrollbar/ScrollBar.h"
#include "meow/ui/widget/menu/item/SpinItem.h"

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
        ID_CTX_MENU,
        ID_SCROLLBAR,
        //
        ID_SAVE_SETS_ITEM,
        ID_TEST_AUDIO_ITEM,
        //
        ID_INTERLEAV_TOG_ITEM,
        ID_VOLUME_SPIN_ITEM,
        ID_HPF_SPIN_ITEM,
        ID_AGC_OUT_SPIN_ITEM,
        //
        ID_AGC_IN_TOG_ITEM,
        ID_AGC_IN_SPIN_ITEM,
        //
        ID_POST_FILTER_TOG_ITEM,
        ID_POST_BBOOST_TOG_ITEM,
        ID_POST_BETA_SPIN_ITEM,
        ID_POST_GAMMA_SPIN_ITEM,
    };

    void showMainTmpl();
    void showContextMenuTmpl();
    void hideContextMenu();
    //
    void loadCodecSettings();
    bool saveCodecSettings();
    void updateFilterSets();
    //
    void clickOk();
    void clickBack();
    void clickUp();
    void clickDown();

private: // TODO вирівняти
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

    unsigned long _last_delay_ts{0};

    bool _audio_test_enabled{false};

    ScrollBar *_scrollbar{nullptr};
    FixedMenu *_main_menu{nullptr};
    FixedMenu *_context_menu{nullptr};
    ToggleSwitch *_test_audio_toggle{nullptr};
    SpinItem *_spin_item{nullptr};
    bool _is_item_locked{false};
};

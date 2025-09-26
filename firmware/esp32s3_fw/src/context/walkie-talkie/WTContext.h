#pragma once
#include <Arduino.h>
#include <codec2.h>
#include "meow/lib/audio/filter/filter.h"
#include "meow/ui/context/IContext.h"
#include "meow/lib/lora/LRE32.h"
#include "meow/lib/lora/LRE220T.h"
#include "LoraSettingsStruct.h"
#include "CodecSettingsStruct.h"
#include "meow/ui/widget/text/Label.h"
#include "meow/ui/widget/menu/FixedMenu.h"
#include "meow/ui/widget/toggle/ToggleSwitch.h"

using namespace meow;

#define E220
// #define E32

#ifdef E220
#define LORA_PACK_SIZE 64
#define CODEC_BUFF_SIZE 36
#define CODEC_BUFF_ITER 6

// #define LORA_PACK_SIZE 128
// #define CODEC_BUFF_SIZE 96
// #define CODEC_BUFF_ITER 16
#else
#define LORA_PACK_SIZE 58
#define CODEC_BUFF_SIZE 30
#define CODEC_BUFF_ITER 5
#endif

class WTContext : public IContext
{
public:
    WTContext();
    virtual ~WTContext();

protected:
    virtual bool loop() override;
    virtual void update() override;

private:
    enum Mode : uint8_t
    {
        MODE_MAIN = 0,
        MODE_SUBCONTEXT,
        MODE_CTX_MENU,
        MODE_SD_UNCONN,
        MODE_LORA_UNCONN,
    };

    enum Widget_ID : uint8_t
    {
        ID_CTX_MENU = 1,
        ID_HEADER_LBL,
        ID_CHNL_TITLE,
        ID_CHNL_VAL,
        ID_ENCRYPT_TITLE,
        ID_ENCRYPT_VAL,
        ID_POWER_TITLE,
        ID_POWER_VAL,
        ID_BAT_TITLE,
        ID_BAT_VAL,
        ID_LBL_STATE,
        ID_ITEM_LORA_SETS,
        ID_ITEM_CODEC_SETS,
        ID_ITEM_ENC,
    };

    void showSDErrTmpl();
    void showLoraErrTmpl();
    void showMainTmpl();
    void showContextMenuTmpl();
    void updateBattVoltage();
    void clickBack();
    void clickOk();
    void clickUp();
    void clickDown();
    void loadSettings();
    void toggleBL();

    static void packSenderTask(void *params);

private:
#ifdef E220
    LRE220T _lora;
#else
    LRE32 _lora;
#endif
    CodecSettings _codec_sets;
    LoraSettings _lora_sets;

    HighPassFilter _hpf;
    SimpleAGC _agc_out;
    SimpleAGC _agc_in;

    String _lora_set_name;

    uint8_t _lora_pack_buf1[LORA_PACK_SIZE];
    uint8_t _lora_pack_buf2[LORA_PACK_SIZE];
    uint8_t _codec_pack_buf[CODEC_BUFF_SIZE];

    volatile xSemaphoreHandle _sync_write_mutex{nullptr};
    xTaskHandle _pack_sender_handle{nullptr};
    struct CODEC2 *_codec{nullptr};
    uint8_t *_lora_pack_write_buf{_lora_pack_buf1};
    uint8_t *_lora_pack_read_buf{_lora_pack_buf2};
    Label *_state_val_lbl{nullptr};
    Label *_encrypt_val_lbl{nullptr};
    Label *_power_val_lbl{nullptr};
    Label *_batt_val_lbl{nullptr};
    FixedMenu *_context_menu{nullptr};
    ToggleSwitch *_enc_toggle{nullptr};
    IContext *_sub_context{nullptr};
    int16_t *_samples_16k_buf{nullptr};
    int16_t *_samples_8k_buf{nullptr};

    unsigned long _upd_batt_ts{0};
    int _samples_per_frame{0};
    int _codec_buf_size{0};

    uint16_t _samples_16k_num{0};

    Mode _mode{MODE_MAIN};
    uint8_t _codec_pack_i{0};
    volatile bool _has_w_packet{false};
    bool _is_encoding_en{false};
    bool _ptt_holded{false};
    bool _is_locked{false};
    bool _is_lora_buff_clean{true};
};
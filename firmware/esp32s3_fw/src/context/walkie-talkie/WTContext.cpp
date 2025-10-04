#include "WTContext.h"
#include "LoraSetsContext.h"
#include "CodecSetsContext.h"
#include "meow/manager/SettingsManager.h"
#include "meow/manager/I2SInManager.h"
#include "meow/manager/I2SOutManager.h"
#include "meow/manager/CoprocessorManager.h"
#include "meow/util/batt_util.h"
#include "meow/util/string_util.h"
#include "meow/util/aes256.h"
#include "meow/ui/widget/menu/item/ToggleItem.h"
#include "../WidgetCreator.h"

const char STR_LORA_STATE_ERR[] = "LoRa не підключено";
const char STR_ENCRYPTION[] = "Шифрування";
const char STR_LORA_SETS[] = "LoRa налаштування";
const char STR_CODEC_SETS[] = "Кодеку налаштування";
const char STR_POWER_TITLE[] = "Потужність:";
const char STR_POWER_MIN_VAL[] = "Мінімальна";
const char STR_POWER_MID_VAL[] = "Середня";
const char STR_POWER_HIGH_VAL[] = "Висока";
const char STR_POWER_MAX_VAL[] = "Максимальна";
const char STR_EMPTY_LORA_SETS[] = "Без налаштувань";
const char STR_BAT_VOLT_TITLE[] = "Напруга:";

#define SAMPLE_RATE 16000U
//
#define LBL_STATE_HEIGHT 20u
#define UPD_DISPLAY_INTERVAL_MS 20000UL

WTContext::WTContext()
{
    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    if (!_sd.isMounted())
    {
        showSDErrTmpl();
        return;
    }

    uint8_t ccpu_cmd_data[2]{CCPU_CMD_PIN_ON, CH_PIN_LORA_PWR};
    _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

    ccpu_cmd_data[1] = CH_PIN_SPK_PWR;
    _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

    ccpu_cmd_data[1] = CH_PIN_MIC_PWR;
    _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

    if (!_lora.init())
    {
        showLoraErrTmpl();
        return;
    }

    loadSettings();
    showMainTmpl();

    _sync_write_mutex = xSemaphoreCreateMutex();
    xTaskCreatePinnedToCore(packSenderTask, "pst", 20 * 512, this, 10, &_pack_sender_handle, 0);
}

void WTContext::loadSettings()
{
    SettingsManager::load(&_codec_sets, sizeof(CodecSettings), STR_CODEC_SETS_NAME, STR_CODEC_SETS_DIR);

    _codec = codec2_create(CODEC2_MODE_1200);
    _samples_per_frame = codec2_samples_per_frame(_codec);
    _codec_buf_size = codec2_bytes_per_frame(_codec);
    _samples_16k_num = _samples_per_frame * 2;
    _samples_16k_buf = static_cast<int16_t *>(malloc(_samples_16k_num * sizeof(int16_t)));
    _samples_8k_buf = static_cast<int16_t *>(malloc(_samples_per_frame * sizeof(int16_t)));

    codec2_set_lpc_post_filter(
        _codec,
        _codec_sets.post_filter_en,
        _codec_sets.post_filter_bboost_en,
        _codec_sets.post_filter_beta,
        _codec_sets.post_filter_gamma);

    _hpf.init(_codec_sets.hpf_out_freq, SAMPLE_RATE);
    _agc_out.setTargetDB(_codec_sets.agc_out_db);
    _agc_in.setTargetDB(_codec_sets.agc_in_db);

    _i2s_in.init(SAMPLE_RATE, _codec_sets.interleaving_en);
    _i2s_in.enable();
    _i2s_out.init(SAMPLE_RATE);
    _i2s_out.enable();

    _lora_set_name = SettingsManager::get(STR_LORA_PRESET, STR_LORA_SETS_DIR);

    if (!_lora_set_name.isEmpty())
    {
        if (!SettingsManager::load(&_lora_sets, sizeof(LoraSettings), _lora_set_name.c_str(), STR_LORA_SETS_DIR))
        {
            _lora_set_name = emptyString;
        }
    }

    _lora.setTransmitPower(static_cast<ILoRa::TransmitPower>(_lora_sets.tr_power));
    _lora.setChannel(_lora_sets.channel);
    _lora.setAirDataRate(ILoRa::AIR_DATA_RATE_4_8K);
    _lora.setPackLen(LORA_PACK_SIZE);
    _lora.writeSettings();
    _lora.setMode(ILoRa::MODE_NORMAL);
}

void WTContext::toggleBL()
{
    _is_locked = !_is_locked;

    if (_is_locked)
    {
        _gui_enabled = false;

        uint8_t ccpu_cmd_data[2]{CCPU_CMD_PIN_OFF, CH_PIN_DISPLAY_PWR};
        _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

        ccpu_cmd_data[0] = CCPU_CMD_BTN_OFF;

        ccpu_cmd_data[1] = BtnID::BTN_BACK;
        _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

        ccpu_cmd_data[1] = BtnID::BTN_LEFT;
        _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

        ccpu_cmd_data[1] = BtnID::BTN_RIGHT;
        _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data));

        ccpu_cmd_data[1] = BtnID::BTN_UP;
        _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

        ccpu_cmd_data[1] = BtnID::BTN_OK;
        _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data));
    }
    else
    {
        _gui_enabled = true;

        uint8_t ccpu_cmd_data[2]{CCPU_CMD_PIN_ON, CH_PIN_DISPLAY_PWR};
        _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

        ccpu_cmd_data[0] = CCPU_CMD_BTN_ON;

        ccpu_cmd_data[1] = BtnID::BTN_BACK;
        _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

        ccpu_cmd_data[1] = BtnID::BTN_LEFT;
        _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

        ccpu_cmd_data[1] = BtnID::BTN_RIGHT;
        _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

        ccpu_cmd_data[1] = BtnID::BTN_UP;
        _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

        ccpu_cmd_data[1] = BtnID::BTN_OK;
        _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data));
    }
}

WTContext::~WTContext()
{
    vTaskDelete(_pack_sender_handle);
    vSemaphoreDelete(_sync_write_mutex);
    _lora.end();

    uint8_t ccpu_cmd_data[2]{CCPU_CMD_PIN_OFF, CH_PIN_LORA_PWR};
    _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

    ccpu_cmd_data[1] = CH_PIN_SPK_PWR;
    _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

    ccpu_cmd_data[1] = CH_PIN_MIC_PWR;
    _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data));

    _i2s_in.deinit();
    _i2s_out.deinit();

    codec2_destroy(_codec);
    free(_samples_8k_buf);
    free(_samples_16k_buf);
}

void WTContext::showLoraErrTmpl()
{
    _mode = MODE_LORA_UNCONN;

    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    _state_val_lbl = creator.getStatusMsgLable(ID_LBL_STATE, STR_LORA_STATE_ERR);
    layout->addWidget(_state_val_lbl);

    setLayout(layout);
}

void WTContext::showSDErrTmpl()
{
    _mode = MODE_SD_UNCONN;

    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    _state_val_lbl = creator.getStatusMsgLable(ID_LBL_STATE, STR_SD_ERR);
    layout->addWidget(_state_val_lbl);

    setLayout(layout);
}

void WTContext::showMainTmpl()
{
    _mode = MODE_MAIN;
    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    // Ім'я налаштувань

    Label *ctx_header_lbl = new Label(ID_HEADER_LBL);
    layout->addWidget(ctx_header_lbl);
    ctx_header_lbl->setWidth(D_WIDTH);
    ctx_header_lbl->setHeight(34);
    ctx_header_lbl->setBackColor(TFT_DARKGREY);
    ctx_header_lbl->setAlign(IWidget::ALIGN_CENTER);
    ctx_header_lbl->setGravity(IWidget::GRAVITY_CENTER);
    ctx_header_lbl->setTextColor(TFT_RED);
    ctx_header_lbl->setTextSize(2);
    ctx_header_lbl->setFontID(4);
    ctx_header_lbl->setTicker(true);

    if (_lora_set_name.isEmpty())
    {
        _lora_set_name = STR_EMPTY_LORA_SETS;
        ctx_header_lbl->setText(_lora_set_name);
    }
    else
    {
        String set_name = _lora_set_name;
        rmFilenameExt(set_name, STR_LORA_SETS_EXT);
        ctx_header_lbl->setText(set_name);
    }

    // Заголовки налаштувань

    Label *lbl_title_chnl = new Label(ID_CHNL_TITLE);
    layout->addWidget(lbl_title_chnl);
    lbl_title_chnl->setText(STR_CHANN_TITLE);
    lbl_title_chnl->setPos(DISPLAY_PADDING, ctx_header_lbl->getYPos() + ctx_header_lbl->getHeight() + DISPLAY_PADDING);
    lbl_title_chnl->setFontID(4);
    lbl_title_chnl->setTextSize(2);
    lbl_title_chnl->setTextColor(TFT_GREEN);
    lbl_title_chnl->initWidthToFit();

    Label *lbl_title_enc = lbl_title_chnl->clone(ID_ENCRYPT_TITLE);
    layout->addWidget(lbl_title_enc);
    lbl_title_enc->setText(STR_ENCRYPT_TITLE);
    lbl_title_enc->setPos(DISPLAY_PADDING, lbl_title_chnl->getYPos() + lbl_title_chnl->getHeight() + DISPLAY_PADDING);
    lbl_title_enc->initWidthToFit();

    Label *lbl_title_pwr = lbl_title_chnl->clone(ID_POWER_TITLE);
    layout->addWidget(lbl_title_pwr);
    lbl_title_pwr->setText(STR_POWER_TITLE);
    lbl_title_pwr->setPos(DISPLAY_PADDING, lbl_title_enc->getYPos() + lbl_title_enc->getHeight() + DISPLAY_PADDING);
    lbl_title_pwr->initWidthToFit();

    Label *lbl_volt_title = lbl_title_chnl->clone(ID_BAT_TITLE);
    layout->addWidget(lbl_volt_title);
    lbl_volt_title->setText(STR_BAT_VOLT_TITLE);
    lbl_volt_title->setPos(DISPLAY_PADDING, lbl_title_pwr->getYPos() + lbl_title_pwr->getHeight() + DISPLAY_PADDING);
    lbl_volt_title->initWidthToFit();

    // Значення

    uint16_t lbl_val_pos = lbl_title_chnl->getWidth();
    if (lbl_title_enc->getWidth() > lbl_val_pos)
        lbl_val_pos = lbl_title_enc->getWidth();
    if (lbl_title_pwr->getWidth() > lbl_val_pos)
        lbl_val_pos = lbl_title_pwr->getWidth();

    lbl_val_pos += (DISPLAY_PADDING * 2);

    Label *lbl_val_chnl = new Label(ID_CHNL_VAL);
    layout->addWidget(lbl_val_chnl);
    lbl_val_chnl->setText(String(_lora_sets.channel));
    lbl_val_chnl->setPos(lbl_val_pos, lbl_title_chnl->getYPos());
    lbl_val_chnl->setTicker(true);
    lbl_val_chnl->setTextColor(TFT_GREEN);
    lbl_val_chnl->initWidthToFit();

    Label *lbl_val_enc = lbl_val_chnl->clone(ID_ENCRYPT_VAL);
    layout->addWidget(lbl_val_enc);

    if (_lora_sets.encrypt_en)
    {
        lbl_val_enc->setText(STR_ENABLED);
        lbl_val_enc->setTextColor(TFT_GREEN);
    }
    else
    {
        lbl_val_enc->setText(STR_DISABLED);
        lbl_val_enc->setTextColor(TFT_RED);
    }

    lbl_val_enc->setPos(lbl_val_pos, lbl_title_enc->getYPos());
    lbl_val_enc->initWidthToFit();

    Label *lbl_val_pwr = lbl_val_chnl->clone(ID_POWER_VAL);
    layout->addWidget(lbl_val_pwr);

    switch (_lora_sets.tr_power)
    {
    case ILoRa::TransmitPower::TR_POWER_MAX:
        lbl_val_pwr->setText(STR_POWER_MAX_VAL);
        break;
    case LRE220T::TransmitPower::TR_POWER_HIGH:
        lbl_val_pwr->setText(STR_POWER_HIGH_VAL);
        break;
    case LRE220T::TransmitPower::TR_POWER_MID:
        lbl_val_pwr->setText(STR_POWER_MID_VAL);
        break;
    case LRE220T::TransmitPower::TR_POWER_LOW:
        lbl_val_pwr->setText(STR_POWER_MIN_VAL);
        break;
    default:
        lbl_val_pwr->setText(STR_NONE);
        break;
    }

    lbl_val_pwr->setPos(lbl_val_pos, lbl_title_pwr->getYPos());
    lbl_val_pwr->initWidthToFit();

    // Акум
    _batt_val_lbl = new Label(ID_BAT_VAL);
    layout->addWidget(_batt_val_lbl);
    _batt_val_lbl->setText(STR_EMPTY_BAT);
    _batt_val_lbl->setWidth(56);
    _batt_val_lbl->setPos(lbl_val_pos, lbl_volt_title->getYPos());
    _batt_val_lbl->setTransparency(true);

    updateBattVoltage();
}

void WTContext::showContextMenuTmpl()
{
    if (_mode != MODE_MAIN)
        return;

    _mode = MODE_CTX_MENU;

    WidgetCreator creator;

    _context_menu = new FixedMenu(ID_CTX_MENU);
    getLayout()->addWidget(_context_menu);
    _context_menu->setItemHeight(18);
    _context_menu->setWidth((float)D_WIDTH / 2);
    _context_menu->setBackColor(TFT_BLACK);
    _context_menu->setBorder(true);
    _context_menu->setBorderColor(TFT_ORANGE);
    _context_menu->setLoopState(true);

    // Шифрування
    ToggleItem *enc_item = new ToggleItem(ID_ITEM_ENC);
    _context_menu->addItem(enc_item);
    enc_item->setFocusBorderColor(TFT_LIME);
    enc_item->setFocusBackColor(COLOR_FOCUS_BACK);
    enc_item->setChangingBorder(true);
    enc_item->setChangingBack(true);

    Label *enc_lbl = creator.getItemLabel(STR_ENCRYPTION, 2);
    enc_item->setLbl(enc_lbl);

    _enc_toggle = new ToggleSwitch(1);
    enc_item->setToggle(_enc_toggle);
    _enc_toggle->setWidth(20);
    _enc_toggle->setHeight(10);
    _enc_toggle->setCornerRadius(2);
    _enc_toggle->setOn(_lora_sets.encrypt_en);

    // Налаштування LoRa
    MenuItem *lora_sets_item = creator.getMenuItem(ID_ITEM_LORA_SETS);
    _context_menu->addItem(lora_sets_item);

    Label *lora_sets_lbl = creator.getItemLabel(STR_LORA_SETS, 2);
    lora_sets_item->setLbl(lora_sets_lbl);
    lora_sets_lbl->setTextOffset(1);

    // Налаштування кодеку
    MenuItem *pref_item = creator.getMenuItem(ID_ITEM_CODEC_SETS);
    _context_menu->addItem(pref_item);

    Label *pref_lbl = creator.getItemLabel(STR_CODEC_SETS, 2);
    pref_item->setLbl(pref_lbl);
    pref_lbl->setTextOffset(1);

    _context_menu->setHeight(_context_menu->getSize() * _context_menu->getItemHeight() + 4);
    _context_menu->setPos(D_WIDTH - _context_menu->getWidth() - DISPLAY_PADDING,
                          D_HEIGHT - DISPLAY_PADDING - _context_menu->getHeight());
}

void WTContext::packSenderTask(void *params)
{
    WTContext &self = *static_cast<WTContext *>(params);

    unsigned long ts = millis();
    while (1)
    {
        if (self._has_w_packet)
        {
            xSemaphoreTake(self._sync_write_mutex, portMAX_DELAY);
            self._has_w_packet = false;
            self._lora.writePacket(self._lora_pack_read_buf);
            xSemaphoreGive(self._sync_write_mutex);
        }

        if (millis() - ts > 2000)
        {
            ts = millis();
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
    }
}

bool WTContext::loop()
{
    if (_mode == MODE_SUBCONTEXT)
    {
        if (!_sub_context->isReleased())
        {
            _sub_context->tick();
            return false;
        }
        else
        {
            delete _sub_context;
            _sub_context = nullptr;
            _mode = MODE_MAIN;

            loadSettings();
            showMainTmpl();
            return true;
        }
    }
    else if (_ptt_holded)
    {
        _i2s_in.read(_samples_16k_buf, _samples_16k_num);
        _hpf.filter(_samples_16k_buf, _samples_16k_num);
        downsampleX2(_samples_16k_buf, _samples_8k_buf, _samples_16k_num);
        _agc_out.process(_samples_8k_buf, _samples_per_frame);
        //
        codec2_encode_1200(_codec, &_codec_pack_buf[_codec_pack_i * _codec_buf_size], _samples_8k_buf);
        ++_codec_pack_i;

        if (_codec_pack_i == CODEC_BUFF_ITER)
        {
            _codec_pack_i = 0;

            if (_lora_sets.encrypt_en)
                aes256Encrypt(_lora_sets.aes_key, _codec_pack_buf, CODEC_BUFF_SIZE, _lora_pack_write_buf);
            else
                memcpy(_lora_pack_write_buf, _codec_pack_buf, CODEC_BUFF_SIZE);

            xSemaphoreTake(_sync_write_mutex, portMAX_DELAY);
            uint8_t *temp_ptr = _lora_pack_write_buf;
            _lora_pack_write_buf = _lora_pack_read_buf;
            _lora_pack_read_buf = temp_ptr;
            _has_w_packet = true;
            xSemaphoreGive(_sync_write_mutex);
        }
    }
    else if (_lora.packAvailable())
    {
        _is_lora_buff_clean = false;

        if (_lora.readPack(_lora_pack_write_buf))
        {
            bool deciphered = true;

            if (_lora_sets.encrypt_en)
                deciphered = aes256Decrypt(_lora_sets.aes_key, _lora_pack_write_buf, CODEC_BUFF_SIZE, _codec_pack_buf);
            else
                memcpy(_codec_pack_buf, _lora_pack_write_buf, CODEC_BUFF_SIZE);

            if (deciphered)
            {
                for (int i = 0; i < CODEC_BUFF_ITER; ++i)
                {
                    codec2_decode_1200(_codec, _samples_8k_buf, &_codec_pack_buf[i * _codec_buf_size]);
                    //
                    if (_codec_sets.agc_in_en)
                        _agc_in.process(_samples_8k_buf, _samples_per_frame);
                    //
                    volume(_samples_8k_buf, _samples_per_frame, _codec_sets.volume);
                    upsampleX2(_samples_8k_buf, _samples_16k_buf, _samples_per_frame);
                    _i2s_out.write(_samples_16k_buf, _samples_16k_num, true);
                }
            }
        }
    }

    return true;
}

void WTContext::update()
{
    if (_mode == MODE_SD_UNCONN || _mode == MODE_LORA_UNCONN)
    {
        if (_input.isReleased(BtnID::BTN_BACK))
        {
            _input.lock(BtnID::BTN_BACK, CLICK_LOCK);
            openContextByID(ID_CONTEXT_MENU);
        }

        return;
    }

    _ptt_holded = _input.isHolded(BtnID::BTN_PTT);

    if (_input.isPressed(BtnID::BTN_BACK))
    {
        _input.lock(BtnID::BTN_BACK, PRESS_LOCK);
        openContextByID(ID_CONTEXT_MENU);
    }
    else if (_input.isPressed(BtnID::BTN_OK))
    {
        _input.lock(BtnID::BTN_OK, PRESS_LOCK);
        showContextMenuTmpl();
    }
    else if (_input.isReleased(BtnID::BTN_OK))
    {
        _input.lock(BtnID::BTN_OK, CLICK_LOCK);
        clickOk();
    }
    else if (_input.isReleased(BtnID::BTN_BACK))
    {
        _input.lock(BtnID::BTN_BACK, CLICK_LOCK);
        clickBack();
    }
    else if (_input.isReleased(BtnID::BTN_UP))
    {
        _input.lock(BtnID::BTN_UP, CLICK_LOCK);
        clickUp();
    }
    else if (_input.isReleased(BtnID::BTN_DOWN))
    {
        _input.lock(BtnID::BTN_DOWN, CLICK_LOCK);
        clickDown();
    }
    else if (_input.isPressed(BtnID::BTN_DOWN))
    {
        _input.lock(BtnID::BTN_DOWN, PRESS_LOCK);
        toggleBL();
    }

    if (_mode == MODE_MAIN && millis() - _upd_batt_ts > UPD_DISPLAY_INTERVAL_MS)
    {
        _upd_batt_ts = millis();
        updateBattVoltage();
    }
}

void WTContext::updateBattVoltage()
{
    float bat_voltage = readBattVoltage();
    String volt_str = String(bat_voltage);
    _batt_val_lbl->setText(volt_str);

    getLayout()->forcedDraw();
}

void WTContext::clickOk()
{
    if (_mode == MODE_CTX_MENU)
    {
        uint16_t id = _context_menu->getCurrItemID();

        if (id == ID_ITEM_ENC)
        {
            _lora_sets.encrypt_en = !_lora_sets.encrypt_en;
            _enc_toggle->setOn(_lora_sets.encrypt_en);
        }
        else if (id == ID_ITEM_LORA_SETS)
        {
            _mode = MODE_SUBCONTEXT;
            getLayout()->delWidgets();
            _sub_context = new LoraSetsContext();
        }
        else if (id == ID_ITEM_CODEC_SETS)
        {
            _mode = MODE_SUBCONTEXT;
            getLayout()->delWidgets();
            _sub_context = new CodecSetsContext();
        }
    }
}

void WTContext::clickUp()
{
    if (_mode == MODE_MAIN)
    {
        if (_codec_sets.volume < 15)
            ++_codec_sets.volume;
    }
    else if (_mode == MODE_CTX_MENU)
    {
        _context_menu->focusUp();
    }
}

void WTContext::clickDown()
{
    if (_mode == MODE_MAIN)
    {
        if (_codec_sets.volume > 1)
            --_codec_sets.volume;
    }
    else if (_mode == MODE_CTX_MENU)
    {
        _context_menu->focusDown();
    }
}

void WTContext::clickBack()
{
    if (_mode == MODE_CTX_MENU)
    {
        showMainTmpl();
    }
}
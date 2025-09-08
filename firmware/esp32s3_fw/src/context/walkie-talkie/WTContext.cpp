#include "WTContext.h"
#include "LoraSetsContext.h"
#include "CodecSetsContext.h"
#include "meow/manager/SettingsManager.h"
#include "meow/manager/I2SInManager.h"
#include "meow/manager/I2SOutManager.h"
#include "meow/manager/CoprocessorManager.h"
#include "meow/util/battery/BatteryUtil.h"
#include "meow/ui/widget/menu/item/ToggleItem.h"
#include "meow/util/crypto/aes256.h"
#include "../resources/ico/battery.h"
#include "../WidgetCreator.h"

const char STR_LORA_STATE_ERR[] = "LoRa не підключено";
const char STR_ENCRYPTION[] = "Шифрування";
const char STR_LORA_SETS[] = "LoRa налаштування";
const char STR_CODEC_SETS[] = "Кодеку налаштування";

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

    SettingsManager::load(&_codec_sets, sizeof(CodecSettings), STR_CODEC_SETS_NAME, STR_CODEC_SETS_DIR);

    // TODO Прочитати імя налаштувань з STR_LORA_PRESET,STR_LORA_SETS_DIR і завантажувати по ньому 
    // SettingsManager::load(&_lora_sets, sizeof(LoraSettings), STR_LORA_SETS_NAME, STR_LORA_SETS_DIR); 

    _i2s_in.init(SAMPLE_RATE, true); // TODO додати налаштування interleaving
    _i2s_in.enable();
    _i2s_out.init(SAMPLE_RATE);
    _i2s_out.enable();

    // TODO Записати налаштування в модуль лори
    showMainTmpl();
}

WTContext::~WTContext()
{
    delete _batt_ico;
    _i2s_in.deinit();
    _i2s_out.deinit();

    uint8_t ccpu_cmd_data[2]{CCPU_CMD_PIN_OFF, CH_PIN_LORA_PWR};
    _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

    ccpu_cmd_data[1] = CH_PIN_SPK_PWR;
    _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);

    ccpu_cmd_data[1] = CH_PIN_MIC_PWR;
    _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);
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

    // Акум
    _batt_val_lbl = new Label(ID_BAT_VAL);
    layout->addWidget(_batt_val_lbl);
    _batt_val_lbl->setText(STR_EMPTY_BAT);
    _batt_val_lbl->setWidth(56);
    _batt_val_lbl->setHeight(35);
    _batt_val_lbl->setAlign(Label::ALIGN_CENTER);
    _batt_val_lbl->setGravity(Label::GRAVITY_CENTER);
    _batt_val_lbl->setPos(_display.width() - TFT_CUTOUT - DISPLAY_PADDING - _batt_val_lbl->getWidth(), DISPLAY_PADDING);
    _batt_val_lbl->setTransparency(true);

    if (!_batt_ico)
    {
        _batt_ico = new Image(1);
        _batt_ico->init(_batt_val_lbl->getWidth(), _batt_val_lbl->getHeight());
        _batt_ico->setSrc(ICO_BATTERY);
        _batt_ico->setTransparency(true);
        _batt_ico->setTranspColor(TFT_TRANSPARENT);
    }

    _batt_val_lbl->setBackImg(_batt_ico);

    // Заголовки

    Label *lbl_title_chnl = new Label(ID_CHNL_TITLE);
    layout->addWidget(lbl_title_chnl);
    lbl_title_chnl->setText(STR_CHANN_TITLE);
    lbl_title_chnl->setPos(DISPLAY_PADDING, _batt_val_lbl->getYPos() + _batt_val_lbl->getHeight() + DISPLAY_PADDING);
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

    switch (_lora_sets.power)
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

    Label *enc_lbl = creator.getItemLabel(STR_ENCRYPTION, 4);
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

    Label *lora_sets_lbl = creator.getItemLabel(STR_LORA_SETS, 4);
    lora_sets_item->setLbl(lora_sets_lbl);
    lora_sets_lbl->setTextOffset(1);

    // Налаштування кодеку
    MenuItem *pref_item = creator.getMenuItem(ID_ITEM_CODEC_SETS);
    _context_menu->addItem(pref_item);

    Label *pref_lbl = creator.getItemLabel(STR_CODEC_SETS, 4);
    pref_item->setLbl(pref_lbl);
    pref_lbl->setTextOffset(1);

    _context_menu->setHeight(_context_menu->getSize() * _context_menu->getItemHeight() + 4);
    _context_menu->setPos(D_WIDTH - _context_menu->getWidth() - DISPLAY_PADDING,
                          D_HEIGHT - DISPLAY_PADDING - _context_menu->getHeight());
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
            // TODO Оновити налаштування лори і кодеку
            showMainTmpl();
            return true;
        }
    }
    else
    {
        if (_ptt_holded == true) // TODO to lora task?
        {
            // TODO надсилаємо пакети
        }
        else
        {
            // TODO отримуємо пакети
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

    if (_input.isHolded(BtnID::BTN_PTT))
        _ptt_holded = true;
    else
        _ptt_holded = false;

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

    if (_mode == MODE_MAIN && millis() - _upd_batt_ts > UPD_DISPLAY_INTERVAL_MS)
    {
        _upd_batt_ts = millis();
        updateBattVoltage();
    }
}

void WTContext::updateBattVoltage()
{
    float bat_voltage = BatteryUtil::readVoltVal();
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
    if (_mode == MODE_MAIN) // TODO гучність
    {
    }
    else if (_mode == MODE_CTX_MENU)
    {
        _context_menu->focusUp();
    }
}

void WTContext::clickDown()
{
    if (_mode == MODE_MAIN) // TODO гучність
    {
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
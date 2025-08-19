#include "LoraContext.h"
#include "../resources/ico/battery.h"
#include "../WidgetCreator.h"
#include "meow/manager/settings/SettingsManager.h"
#include "meow/util/battery/BatteryUtil.h"
#include "meow/manager/coprocessor/CoprocessorManager.h"

const char STR_LORA_STATE_ERR[] = "LoRa не підключено";
const char STR_TR_TEST[] = "Тест мікрофону"; // TODO

#define LBL_STATE_HEIGHT 20u
#define UPD_DISPLAY_INTERVAL_MS 20000UL

namespace meow
{
    LoraContext::LoraContext()
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

        if (!_lora.init())
        {
            showLoraErrTmpl();
            return;
        }

        loadLoraSettings();
        showMainTmpl();
    }

    LoraContext::~LoraContext()
    {
        uint8_t ccpu_cmd_data[2]{CCPU_CMD_PIN_OFF, CH_PIN_LORA_PWR};
        _ccpu.sendCmd(ccpu_cmd_data, sizeof(ccpu_cmd_data), 2);
        delete _batt_ico;
    }

    void LoraContext::showLoraErrTmpl()
    {
        _mode = MODE_LORA_UNCONN;

        WidgetCreator creator;
        EmptyLayout *layout = creator.getEmptyLayout();
        _state_val_lbl = creator.getStatusMsgLable(ID_LBL_STATE, STR_LORA_STATE_ERR);
        layout->addWidget(_state_val_lbl);

        setLayout(layout);
    }

    void LoraContext::showSDErrTmpl()
    {
        _mode = MODE_SD_UNCONN;

        WidgetCreator creator;
        EmptyLayout *layout = creator.getEmptyLayout();
        _state_val_lbl = creator.getStatusMsgLable(ID_LBL_STATE, STR_SD_ERR);
        layout->addWidget(_state_val_lbl);

        setLayout(layout);
    }

    void LoraContext::showMainTmpl()
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
    }

    void LoraContext::loadLoraSettings()
    {
        String sets_path = SettingsManager::getSettingsFilePath("");
        sets_path += STR_LORA_SETS_DIR;

        if (!_fs.dirExist(sets_path.c_str(), true))
            return;

        sets_path += STR_LORA_SETS_NAME;

        if (_fs.fileExist(sets_path.c_str()))
            _fs.readFile(sets_path.c_str(), &_lora_sets, sizeof(_lora_sets));
    }

    bool LoraContext::loop()
    {
        return true;
    }

    void LoraContext::update()
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

        if (_input.isPressed(BtnID::BTN_BACK))
        {
            _input.lock(BtnID::BTN_BACK, PRESS_LOCK);
            openContextByID(ID_CONTEXT_MENU);
        }

        if (millis() - _upd_batt_ts > UPD_DISPLAY_INTERVAL_MS)
        {
            _upd_batt_ts = millis();
            updateBattVoltage();
        }
    }

    void LoraContext::updateBattVoltage()
    {
        float bat_voltage = BatteryUtil::readVoltVal();
        String volt_str = String(bat_voltage);
        _batt_val_lbl->setText(volt_str);

        getLayout()->forcedDraw();
    }
}
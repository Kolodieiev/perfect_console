#include "CodecSetsContext.h"
#include "meow/ui/widget/menu/item/ToggleItem.h"
#include "../WidgetCreator.h"
#include "meow/manager/SettingsManager.h"

const char STR_VOLUME_DB[] = "Підсилення";
const char STR_INTERLEAV_EN[] = "Чергування (перезапусти апп)";
//
const char STR_HPS_FREQ[] = "HPF out hz";
const char STR_AGC_OUT_DB[] = "AGC out db";
//
const char STR_POST_FILTER_EN[] = "Post-filtering in";
const char STR_POST_B_BOOST[] = "Post Bass boost";
const char STR_POST_BETTA[] = "Post Betta";
const char STR_POST_GAMMA[] = "Post Gamma";
//
const char STR_AGC_IN_EN[] = "AGC in";
const char STR_AGC_IN_DB[] = "AGC in db";
//
const char STR_SAVE_SETS[] = "Зберегти налаштування";
const char STR_AUDIO_TEST[] = "Тест аудіо";

#define SAMPLE_RATE 16000U
#define MENU_ITEMS_ON_SCREEN 5U

CodecSetsContext::CodecSetsContext()
{
    WidgetCreator creator;
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);

    SettingsManager::load(&_codec_sets, sizeof(CodecSettings), STR_CODEC_SETS_NAME, STR_CODEC_SETS_DIR);

    _codec = codec2_create(CODEC2_MODE_1200);
    _samples_per_frame = codec2_samples_per_frame(_codec);
    _codec_buf_size = codec2_bytes_per_frame(_codec);
    _codec_buf = static_cast<uint8_t *>(malloc(_codec_buf_size));
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
    if (_audio_test_enabled)
    {
        _i2s_in.read(_samples_16k_buf, _samples_16k_num);
        _hpf.filter(_samples_16k_buf, _samples_16k_num);
        downsampleX2(_samples_16k_buf, _samples_8k_buf, _samples_16k_num);
        _agc_out.process(_samples_8k_buf, _samples_per_frame);
        //
        codec2_encode_1200(_codec, _codec_buf, _samples_8k_buf);
        codec2_decode_1200(_codec, _samples_8k_buf, _codec_buf);
        //
        if (_codec_sets.agc_in_en)
            _agc_in.process(_samples_8k_buf, _samples_per_frame);
        //
        volume(_samples_8k_buf, _samples_per_frame, _codec_sets.volume);
        upsampleX2(_samples_8k_buf, _samples_16k_buf, _samples_per_frame);
        _i2s_out.write(_samples_16k_buf, _samples_16k_num, true);
    }

    return true;
}

void CodecSetsContext::update()
{
    if (_input.isHolded(BtnID::BTN_UP))
    {
        _input.lock(BtnID::BTN_UP, HOLD_LOCK);
        clickUp();
    }
    else if (_input.isHolded(BtnID::BTN_DOWN))
    {
        _input.lock(BtnID::BTN_DOWN, HOLD_LOCK);
        clickDown();
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
    else if (_input.isPressed(BtnID::BTN_OK))
    {
        _input.lock(BtnID::BTN_OK, PRESS_LOCK);
        showContextMenuTmpl();
    }
    else if (_input.isPressed(BtnID::BTN_BACK))
    {
        _input.lock(BtnID::BTN_BACK, PRESS_LOCK);
        release();
    }
}

void CodecSetsContext::showMainTmpl()
{
    WidgetCreator creator;
    //
    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);
    //
    _main_menu = new FixedMenu(ID_MAIN_MENU);
    layout->addWidget(_main_menu);
    _main_menu->setBackColor(COLOR_MAIN_BACK);
    _main_menu->setWidth(_display.width() - SCROLLBAR_WIDTH - DISPLAY_PADDING * 2);
    _main_menu->setHeight(_display.height() - TFT_CUTOUT * 2);
    _main_menu->setItemHeight((_main_menu->getHeight() - 2) / MENU_ITEMS_ON_SCREEN);
    _main_menu->setPos(DISPLAY_PADDING, TFT_CUTOUT);
    //
    _scrollbar = new ScrollBar(ID_SCROLLBAR);
    layout->addWidget(_scrollbar);
    _scrollbar->setWidth(SCROLLBAR_WIDTH);
    _scrollbar->setHeight(_main_menu->getHeight());
    _scrollbar->setPos(_main_menu->getWidth() + _main_menu->getXPos(), _main_menu->getYPos());
    _scrollbar->setBackColor(COLOR_MAIN_BACK);

    // VOLUME
    SpinItem *volume_item = new SpinItem(ID_VOLUME_SPIN_ITEM);
    _main_menu->addItem(volume_item);
    volume_item->setFocusBorderColor(TFT_LIME);
    volume_item->setFocusBackColor(COLOR_FOCUS_BACK);
    volume_item->setChangingBorder(true);
    volume_item->setChangingBack(true);

    Label *volume_lbl = creator.getItemLabel(STR_VOLUME_DB, 4, 2);
    volume_item->setLbl(volume_lbl);

    SpinBox *volume_spin = volume_item->getSpin();
    volume_spin->setMax(30);
    volume_spin->setMin(1);
    volume_spin->setStep(1);
    volume_spin->setValue(_codec_sets.volume);
    volume_spin->setWidth(50);
    volume_spin->setFontID(4);
    volume_spin->setTextSize(2);
    volume_spin->setBackColor(TFT_WHITE);
    volume_spin->setTextColor(TFT_BLACK);
    volume_spin->setCornerRadius(5);

    // HPF
    SpinItem *hpf_freq_item = volume_item->clone(ID_HPF_SPIN_ITEM);
    _main_menu->addItem(hpf_freq_item);

    Label *hpf_freq_lbl = hpf_freq_item->getLbl();
    hpf_freq_lbl->setText(STR_HPS_FREQ);

    SpinBox *hpf_freq_spin = hpf_freq_item->getSpin();
    hpf_freq_spin->setMax(500);
    hpf_freq_spin->setMin(80);
    hpf_freq_spin->setStep(10);
    hpf_freq_spin->setValue(_codec_sets.hpf_out_freq);

    // AGC
    SpinItem *agc_db_item = hpf_freq_item->clone(ID_AGC_OUT_SPIN_ITEM);
    _main_menu->addItem(agc_db_item);
    agc_db_item->getLbl()->setText(STR_AGC_OUT_DB);

    SpinBox *agc_db_spin = agc_db_item->getSpin();
    agc_db_spin->setMax(0);
    agc_db_spin->setMin(-15);
    agc_db_spin->setStep(1);
    agc_db_spin->setValue(_codec_sets.agc_out_db);

    // AGC_IN TOGGLE
    ToggleItem *agc_in_toggle_item = new ToggleItem(ID_AGC_IN_TOG_ITEM);
    _main_menu->addItem(agc_in_toggle_item);
    agc_in_toggle_item->setFocusBorderColor(TFT_LIME);
    agc_in_toggle_item->setFocusBackColor(COLOR_FOCUS_BACK);
    agc_in_toggle_item->setChangingBorder(true);
    agc_in_toggle_item->setChangingBack(true);

    Label *agc_in_toggle_lbl = creator.getItemLabel(STR_AGC_IN_EN, 4, 2);
    agc_in_toggle_item->setLbl(agc_in_toggle_lbl);

    ToggleSwitch *toggle_agc_in = new ToggleSwitch(1);
    agc_in_toggle_item->setToggle(toggle_agc_in);
    toggle_agc_in->setWidth(40);
    toggle_agc_in->setHeight(20);
    toggle_agc_in->setCornerRadius(7);
    toggle_agc_in->setOn(_codec_sets.agc_in_en);

    // AGC_IN SPIN
    SpinItem *agc_in_db_item = agc_db_item->clone(ID_AGC_IN_SPIN_ITEM);
    _main_menu->addItem(agc_in_db_item);
    agc_in_db_item->getLbl()->setText(STR_AGC_IN_DB);

    SpinBox *agc_in_db_spin = agc_in_db_item->getSpin();
    agc_in_db_spin->setValue(_codec_sets.agc_in_db);

    // POST-FILTERING TOGGLE
    ToggleItem *post_filter_toggle_item = agc_in_toggle_item->clone(ID_POST_FILTER_TOG_ITEM);
    _main_menu->addItem(post_filter_toggle_item);
    post_filter_toggle_item->getLbl()->setText(STR_POST_FILTER_EN);
    post_filter_toggle_item->setOn(_codec_sets.post_filter_en);

    // POST BASS BOOST SPIN
    ToggleItem *post_bboost_tog_item = agc_in_toggle_item->clone(ID_POST_BBOOST_TOG_ITEM);
    _main_menu->addItem(post_bboost_tog_item);
    post_bboost_tog_item->getLbl()->setText(STR_POST_B_BOOST);
    post_bboost_tog_item->setOn(_codec_sets.post_filter_bboost_en);

    // POST BETA SPIN
    SpinItem *post_beta_item = hpf_freq_item->clone(ID_POST_BETA_SPIN_ITEM);
    _main_menu->addItem(post_beta_item);

    post_beta_item->getLbl()->setText(STR_POST_BETTA);

    SpinBox *post_beta_spin = post_beta_item->getSpin();
    post_beta_spin->setType(SpinBox::TYPE_FLOAT);
    post_beta_spin->setMax(1);
    post_beta_spin->setMin(0);
    post_beta_spin->setStep(0.1f);
    post_beta_spin->setValue(_codec_sets.post_filter_beta);

    // POST GAMMA SPIN
    SpinItem *post_gamma_item = post_beta_item->clone(ID_POST_GAMMA_SPIN_ITEM);
    _main_menu->addItem(post_gamma_item);
    post_gamma_item->getLbl()->setText(STR_POST_GAMMA);
    post_gamma_item->getSpin()->setValue(_codec_sets.post_filter_gamma);

    // INTERLEAVING TOGGLE
    ToggleItem *interleaving_toggle_item = agc_in_toggle_item->clone(ID_INTERLEAV_TOG_ITEM);
    _main_menu->addItem(interleaving_toggle_item);
    interleaving_toggle_item->getLbl()->setText(STR_INTERLEAV_EN);
    interleaving_toggle_item->setOn(_codec_sets.interleaving_en);

    _scrollbar->setMax(_main_menu->getSize());
}

void CodecSetsContext::showContextMenuTmpl()
{
    if (_mode != MODE_MAIN)
        return;

    _mode = MODE_CTX_MENU;

    _main_menu->disable();

    WidgetCreator creator;

    _context_menu = new FixedMenu(ID_CTX_MENU);
    getLayout()->addWidget(_context_menu);
    _context_menu->setItemHeight(18);
    _context_menu->setWidth((float)D_WIDTH / 2);
    _context_menu->setBackColor(TFT_BLACK);
    _context_menu->setBorder(true);
    _context_menu->setBorderColor(TFT_ORANGE);
    _context_menu->setLoopState(true);

    // Зберегти налаштування
    MenuItem *save_sets_item = creator.getMenuItem(ID_SAVE_SETS_ITEM);
    _context_menu->addItem(save_sets_item);

    Label *save_sets_lbl = creator.getItemLabel(STR_SAVE_SETS, 4);
    save_sets_item->setLbl(save_sets_lbl);
    save_sets_lbl->setTextOffset(1);

    // Тест аудіо
    ToggleItem *test_audio_item = new ToggleItem(ID_TEST_AUDIO_ITEM);
    _context_menu->addItem(test_audio_item);
    test_audio_item->setFocusBorderColor(TFT_LIME);
    test_audio_item->setFocusBackColor(COLOR_FOCUS_BACK);
    test_audio_item->setChangingBorder(true);
    test_audio_item->setChangingBack(true);

    Label *test_audio_lbl = creator.getItemLabel(STR_AUDIO_TEST, 4);
    test_audio_item->setLbl(test_audio_lbl);

    _test_audio_toggle = new ToggleSwitch(1);
    test_audio_item->setToggle(_test_audio_toggle);
    _test_audio_toggle->setWidth(20);
    _test_audio_toggle->setHeight(10);
    _test_audio_toggle->setCornerRadius(2);
    _test_audio_toggle->setOn(_audio_test_enabled);

    _context_menu->setHeight(_context_menu->getSize() * _context_menu->getItemHeight() + 4);
    _context_menu->setPos(D_WIDTH - _context_menu->getWidth() - DISPLAY_PADDING,
                          D_HEIGHT - DISPLAY_PADDING - _context_menu->getHeight());
}

void CodecSetsContext::hideContextMenu()
{
    getLayout()->delWidgetByID(ID_CTX_MENU);
    getLayout()->forcedDraw();
    _main_menu->enable();
    _mode = MODE_MAIN;
}

void CodecSetsContext::updateFilterSets()
{
    const ToggleItem *agc_in_toggle = _main_menu->getWidgetByID(ID_AGC_IN_TOG_ITEM)->castTo<ToggleItem>();
    const ToggleItem *post_filter_toggle = _main_menu->getWidgetByID(ID_POST_FILTER_TOG_ITEM)->castTo<ToggleItem>();
    const ToggleItem *bbost_toggle = _main_menu->getWidgetByID(ID_POST_BBOOST_TOG_ITEM)->castTo<ToggleItem>();
    const ToggleItem *interleav_toggle = _main_menu->getWidgetByID(ID_INTERLEAV_TOG_ITEM)->castTo<ToggleItem>();

    const SpinItem *volume_spin = _main_menu->getWidgetByID(ID_VOLUME_SPIN_ITEM)->castTo<SpinItem>();
    const SpinItem *hpf_spin = _main_menu->getWidgetByID(ID_HPF_SPIN_ITEM)->castTo<SpinItem>();
    const SpinItem *agc_out_spin = _main_menu->getWidgetByID(ID_AGC_OUT_SPIN_ITEM)->castTo<SpinItem>();
    const SpinItem *agc_in_spin = _main_menu->getWidgetByID(ID_AGC_IN_SPIN_ITEM)->castTo<SpinItem>();

    const SpinItem *beta_spin = _main_menu->getWidgetByID(ID_POST_BETA_SPIN_ITEM)->castTo<SpinItem>();
    const SpinItem *gamma_spin = _main_menu->getWidgetByID(ID_POST_GAMMA_SPIN_ITEM)->castTo<SpinItem>();

    _codec_sets.interleaving_en = interleav_toggle->isOn();

    _codec_sets.agc_in_en = agc_in_toggle->isOn();
    _codec_sets.post_filter_en = post_filter_toggle->isOn();

    _codec_sets.volume = volume_spin->getValue();
    _codec_sets.hpf_out_freq = hpf_spin->getValue();
    _codec_sets.agc_out_db = agc_out_spin->getValue();
    _codec_sets.agc_in_db = agc_in_spin->getValue();

    _codec_sets.post_filter_bboost_en = bbost_toggle->isOn();
    _codec_sets.post_filter_beta = beta_spin->getValue();
    _codec_sets.post_filter_gamma = gamma_spin->getValue();

    codec2_set_lpc_post_filter(
        _codec,
        _codec_sets.post_filter_en,
        _codec_sets.post_filter_bboost_en,
        _codec_sets.post_filter_beta,
        _codec_sets.post_filter_gamma);

    _hpf.init(_codec_sets.hpf_out_freq, SAMPLE_RATE);
    _agc_out.setTargetDB(_codec_sets.agc_out_db);
    _agc_in.setTargetDB(_codec_sets.agc_in_db);
}

void CodecSetsContext::clickOk()
{
    if (_mode == MODE_MAIN)
    {
        if (_is_item_locked)
        {
            _is_item_locked = false;
            _spin_item->setBorderColor(TFT_LIME);
            return;
        }

        IWidget::TypeID t_id = _main_menu->getCurrItem()->getTypeID();

        if (t_id == IWidget::TYPE_ID_SPIN_ITEM)
        {
            _is_item_locked = true;
            _spin_item = _main_menu->getCurrItem()->castTo<SpinItem>();
            _spin_item->setBorderColor(TFT_RED);
        }
        else if (t_id == IWidget::TYPE_ID_TOGGLE_ITEM)
        {
            ToggleItem *_toggle_item = _main_menu->getCurrItem()->castTo<ToggleItem>();
            _toggle_item->toggle();
            updateFilterSets();
        }
    }
    else if (_mode == MODE_CTX_MENU)
    {
        uint16_t item_id = _context_menu->getCurrItemID();

        if (item_id == ID_SAVE_SETS_ITEM)
        {
            if (SettingsManager::save(&_codec_sets, sizeof(CodecSettings), STR_CODEC_SETS_NAME, STR_CODEC_SETS_DIR))
                showToast(STR_SUCCSESS);
            else
                showToast(STR_FAIL);

            hideContextMenu();
        }
        else if (item_id == ID_TEST_AUDIO_ITEM)
        {
            _audio_test_enabled = !_audio_test_enabled;
            _test_audio_toggle->setOn(_audio_test_enabled);
        }
    }
}

void CodecSetsContext::clickBack()
{
    if (_mode == MODE_CTX_MENU)
    {
        hideContextMenu();
    }
}

void CodecSetsContext::clickUp()
{
    if (_mode == MODE_MAIN)
    {
        if (!_is_item_locked)
        {
            _main_menu->focusUp();
            _scrollbar->scrollUp();
        }
        else
        {
            _spin_item->up();
            updateFilterSets();
        }
    }
    else if (_mode == MODE_CTX_MENU)
    {
        _context_menu->focusUp();
    }
}

void CodecSetsContext::clickDown()
{
    if (_mode == MODE_MAIN)
    {
        if (!_is_item_locked)
        {
            _main_menu->focusDown();
            _scrollbar->scrollDown();
        }
        else
        {
            _spin_item->down();
            updateFilterSets();
        }
    }
    else if (_mode == MODE_CTX_MENU)
    {
        _context_menu->focusDown();
    }
}
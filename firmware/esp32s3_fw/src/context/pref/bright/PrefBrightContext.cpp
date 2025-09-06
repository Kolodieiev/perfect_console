#include "PrefBrightContext.h"
#include "meow/manager/SettingsManager.h"

#include "../../WidgetCreator.h"

#define MAX_BRIGHT 250
#define BRIGHT_STEP 10

PrefBrightContext::PrefBrightContext()
{
    String bright = SettingsManager::get(STR_PREF_BRIGHT);

    if (bright.equals(""))
        _old_bright = 100;
    else
        _old_bright = atoi(bright.c_str());

    WidgetCreator creator;

    EmptyLayout *layout = creator.getEmptyLayout();
    setLayout(layout);
    //
    Label *header_lbl = new Label(ID_HEADER);
    layout->addWidget(header_lbl);
    header_lbl->setText(STR_BRIGHT);
    header_lbl->setWidth(_display.width());
    header_lbl->setBackColor(COLOR_MAIN_BACK);
    header_lbl->setTextColor(TFT_WHITE);
    header_lbl->setAlign(IWidget::ALIGN_CENTER);
    header_lbl->setGravity(IWidget::GRAVITY_CENTER);
    //
    _progress = new ProgressBar(ID_PROGRESSBAR);
    layout->addWidget(_progress);
    _progress->setBackColor(TFT_BLACK);
    _progress->setProgressColor(TFT_ORANGE);
    _progress->setBorderColor(TFT_WHITE);
    _progress->setMax(MAX_BRIGHT);
    _progress->setWidth(_display.width() - 5 * 8);
    _progress->setHeight(20);
    _progress->setProgress(_old_bright);
    _progress->setPos((_display.width() - _progress->getWidth()) / 2, 40);
}

bool PrefBrightContext::loop()
{
    return true;
}

void PrefBrightContext::update()
{
    if (_input.isReleased(BtnID::BTN_OK))
    {
        _input.lock(BtnID::BTN_OK, CLICK_LOCK);

        SettingsManager::set(STR_PREF_BRIGHT, String(_progress->getProgress()).c_str());
        release();
    }
    else if (_input.isReleased(BtnID::BTN_BACK))
    {
        _input.lock(BtnID::BTN_BACK, CLICK_LOCK);

        _display.setBrightness(_old_bright);
        release();
    }
    else if (_input.isHolded(BtnID::BTN_UP))
    {
        _input.lock(BtnID::BTN_UP, HOLD_LOCK);

        uint16_t cur_progress = _progress->getProgress();

        if (cur_progress < MAX_BRIGHT)
        {
            cur_progress += BRIGHT_STEP;
            _progress->setProgress(cur_progress);
            _display.setBrightness(cur_progress);
        }
    }
    else if (_input.isHolded(BtnID::BTN_DOWN))
    {
        _input.lock(BtnID::BTN_DOWN, HOLD_LOCK);

        uint16_t cur_progress = _progress->getProgress();
        if (cur_progress > BRIGHT_STEP * 2)
        {
            cur_progress -= BRIGHT_STEP;
            _progress->setProgress(cur_progress);
            _display.setBrightness(cur_progress);
        }
    }
}
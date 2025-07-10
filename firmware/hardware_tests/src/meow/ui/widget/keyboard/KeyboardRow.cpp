#pragma GCC optimize("O3")

#include "KeyboardRow.h"
#include "../text/Label.h"

namespace meow
{
    KeyboardRow::KeyboardRow(uint16_t widget_ID) : IWidgetContainer(widget_ID, TYPE_ID_KB_ROW) {}

    KeyboardRow *KeyboardRow::clone(uint16_t id) const
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        try
        {
            KeyboardRow *cln = new KeyboardRow(id);
            cln->_has_border = _has_border;
            cln->_x_pos = _x_pos;
            cln->_y_pos = _y_pos;
            cln->_width = _width;
            cln->_height = _height;
            cln->_back_color = _back_color;
            cln->_border_color = _border_color;
            cln->_corner_radius = _corner_radius;
            cln->_btn_height = _btn_height;
            cln->_btn_width = _btn_width;
            cln->_is_transparent = _is_transparent;
            cln->_visibility = _visibility;
            cln->_has_focus = _has_focus;
            cln->_old_border_state = _old_border_state;
            cln->_need_clear_border = _need_clear_border;
            cln->_need_change_border = _need_change_border;
            cln->_need_change_back = _need_change_back;
            cln->_focus_border_color = _focus_border_color;
            cln->_old_border_color = _old_border_color;
            cln->_focus_back_color = _focus_back_color;
            cln->_old_back_color = _old_back_color;
            cln->_parent = _parent;

            // cppcheck-suppress constVariableReference
            for (const auto &widget_ptr : _widgets)
                cln->addWidget(widget_ptr->clone(widget_ptr->getID()));

            xSemaphoreGive(_widg_mutex);
            return cln;
        }
        catch (const std::bad_alloc &e)
        {
            log_e("%s", e.what());
            esp_restart();
        }
    }

    uint16_t KeyboardRow::getCurrBtnID() const
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);
        uint16_t id = getFocusBtn()->getID();
        xSemaphoreGive(_widg_mutex);
        return id;
    }

    String KeyboardRow::getCurrBtnTxt() const
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);
        const Label *lbl = getFocusBtn()->castTo<Label>();
        xSemaphoreGive(_widg_mutex);
        return lbl->getText();
    }

    bool KeyboardRow::focusUp()
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        if (_cur_focus_pos > 0)
        {
            IWidget *btn = getFocusBtn();
            btn->removeFocus();
            --_cur_focus_pos;
            btn = getFocusBtn();
            btn->setFocus();

            xSemaphoreGive(_widg_mutex);
            return true;
        }

        xSemaphoreGive(_widg_mutex);
        return false;
    }

    bool KeyboardRow::focusDown()
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        if (!_widgets.empty() && _cur_focus_pos < _widgets.size() - 1)
        {
            IWidget *btn = getFocusBtn();
            btn->removeFocus();
            ++_cur_focus_pos;
            btn = getFocusBtn();
            btn->setFocus();

            xSemaphoreGive(_widg_mutex);
            return true;
        }

        xSemaphoreGive(_widg_mutex);
        return false;
    }

    void KeyboardRow::setFocus(uint16_t pos)
    {
        if (_widgets.empty())
            return;

        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        IWidget *btn = getFocusBtn();
        btn->removeFocus();

        if (pos > _widgets.size() - 1)
            _cur_focus_pos = _widgets.size() - 1;
        else
            _cur_focus_pos = pos;

        btn = getFocusBtn();
        btn->setFocus();

        xSemaphoreGive(_widg_mutex);
    }

    void KeyboardRow::removeFocus()
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        getFocusBtn()->removeFocus();
        _cur_focus_pos = 0;

        xSemaphoreGive(_widg_mutex);
    }

    IWidget *KeyboardRow::getFocusBtn() const
    {
        if (_widgets.empty())
        {
            log_e("Не додано жодної кнопки");
            esp_restart();
        }

        IWidget *item = _widgets[_cur_focus_pos];

        if (!item)
        {
            log_e("Кнопку не знайдено");
            esp_restart();
        }

        return item;
    }

    void KeyboardRow::onDraw()
    {
        xSemaphoreTake(_widg_mutex, portMAX_DELAY);

        if (!_is_changed)
        {
            if (_visibility != INVISIBLE && _is_enabled)
                for (size_t i{0}; i < _widgets.size(); ++i)
                    _widgets[i]->onDraw();
        }
        else
        {
            _is_changed = false;

            if (_visibility == INVISIBLE)
            {
                if (!_is_transparent)
                    hide();
                xSemaphoreGive(_widg_mutex);
                return;
            }

            if (!_is_transparent)
                clear();

            if (_widgets.empty())
            {
                xSemaphoreGive(_widg_mutex);
                return;
            }

            uint16_t step{0};

            if (_btn_width * _widgets.size() < _width)
                step = (float)(_width - _btn_width * _widgets.size()) / (_widgets.size() + 1);

            uint16_t x = step;
            uint16_t y = (float)(_height - _btn_height) / 2;

            for (size_t i{0}; i < _widgets.size(); ++i)
            {
                _widgets[i]->setPos(x, y);
                _widgets[i]->setWidth(_btn_width);
                _widgets[i]->setHeight(_btn_height);
                _widgets[i]->onDraw();

                x += _btn_width + step;
            }
        }

        xSemaphoreGive(_widg_mutex);
    }
}
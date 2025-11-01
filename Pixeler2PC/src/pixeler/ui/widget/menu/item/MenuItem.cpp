#pragma GCC optimize("O3")
#include "MenuItem.h"

namespace pixeler
{
  MenuItem::MenuItem(uint16_t widget_ID, IWidget::TypeID type_ID) : IWidget(widget_ID, type_ID)
  {
    setLbl(new Label(1));
  }

  MenuItem::~MenuItem()
  {
    delete _label;
  }

  String MenuItem::getText() const
  {
    return _label->getText();
  }

  void MenuItem::setText(const String& text)
  {
    _label->setText(text);
  }

  void MenuItem::onDraw()
  {
    if (!_is_changed)
    {
      if (_img)
        _img->onDraw();
      _label->onDraw();
      return;
    }

    _is_changed = false;

    clear();

    uint8_t img_width{0};
    if (_img)
    {
      _img->setParent(this);
      img_width = _img->getWidth() + ITEM_PADDING;
      _img->setPos(ITEM_PADDING, (_height - _img->getHeight()) * 0.5);

      uint16_t bk_color = _back_color;

      if (_has_focus && _need_change_back)
        _img->setBackColor(_focus_back_color);

      _img->setBackColor(bk_color);

      _img->onDraw();
    }

    _label->setHeight(_height - 2);
    _label->setPos(img_width + ITEM_PADDING, 1);
    _label->setWidth(_width - ITEM_PADDING * 2 - img_width);

    if (_has_focus)
      _label->setFocus();
    else
      _label->removeFocus();

    _label->onDraw();
  }

  MenuItem* MenuItem::clone(uint16_t id) const
  {
    try
    {
      MenuItem* cln = new MenuItem(id);
      cln->_has_border = _has_border;
      cln->_x_pos = _x_pos;
      cln->_y_pos = _y_pos;
      cln->_width = _width;
      cln->_height = _height;
      cln->_back_color = _back_color;
      cln->_border_color = _border_color;
      cln->_corner_radius = _corner_radius;
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

      if (_img)
        cln->setImg(_img->clone(_img->getID()));
      cln->setLbl(_label->clone(_label->getID()));

      return cln;
    }
    catch (const std::bad_alloc& e)
    {
      log_e("%s", e.what());
      esp_restart();
    }
  }

  void MenuItem::setImg(Image* img_ptr)
  {
    if (img_ptr == _img)
      return;

    delete _img;
    _img = img_ptr;

    _is_changed = true;
  }

  void MenuItem::setLbl(Label* lbl_ptr)
  {
    if (!lbl_ptr)
    {
      log_e("Label не може бути null");
      esp_restart();
    }

    if (lbl_ptr == _label)
      return;

    delete _label;
    _label = lbl_ptr;

    _is_changed = true;

    _label->setParent(this);
    _label->setBorder(false);
    _label->setChangingBorder(false);
    _label->setGravity(GRAVITY_CENTER);

    _label->setBackColor(_back_color);
    _label->setFocusBackColor(_focus_back_color);
    _label->setChangingBack(_need_change_back);
    _label->setTransparency(_is_transparent);
  }
}  // namespace pixeler
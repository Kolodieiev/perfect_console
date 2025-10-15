#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>

#include "../IWidget.h"
#include "Label.h"

namespace pixeler
{
  // TODO add multiline
  // TODO add hint

  class TextBox : public Label
  {
  public:
    enum FieldType : uint8_t
    {
      TYPE_TEXT = 0,
      TYPE_PASSWORD
    };

    explicit TextBox(uint16_t widget_ID, IWidget::TypeID type_ID = TYPE_ID_TEXTBOX);
    virtual ~TextBox() {}

    /**
     * @brief Викликає процедуру малювання віджета на дисплей.
     * Якщо віджет не було змінено, він автоматично пропустить перемальовування.
     *
     */
    virtual void onDraw() override;

    /**
     * @brief Повертає вказівник на глибоку копію віджета.
     *
     * @param id Ідентифікатор, який буде присвоєно новому віджету.
     * @return TextBox*
     */
    virtual TextBox* clone(uint16_t id) const override;

    /**
     * @brief Повертає ідентифікатор типу.
     * Використовується в системі приведення типу.
     *
     * @return constexpr TypeID
     */
    static constexpr TypeID staticType()
    {
      return TypeID::TYPE_ID_TEXTBOX;
    }

    /**
     * @brief Встановлює тип текстового поля.
     *
     * @param type Може мати значення:
     * TYPE_TEXT - текст відображається регулярними символами.
     * TYPE_PASSWORD - усі символи замінюються на *.
     */
    void setType(FieldType type)
    {
      _type = type;
    }

    /**
     * @brief Повертає тип текстового поля.
     *
     * @return FieldType
     */
    FieldType getType() const
    {
      return _type;
    }

    /**
     * @brief Видаляє останній символ з рядка, що знаходиться в текстовому полі.
     *
     * @return true
     * @return false
     */
    bool removeLastChar();

    /**
     * @brief Додає послідовність символів в кінець рядка текстового поля.
     *
     * @param ch
     */
    void addChars(const char* ch);

  private:
    using Label::getAutoscrollDelay;
    using Label::hasAutoscroll;
    using Label::hasAutoscrollInFocus;
    using Label::initWidthToFit;
    using Label::setAutoscroll;
    using Label::setAutoscrollDelay;
    using Label::setAutoscrollInFocus;
    using Label::setGravity;
    using Label::updateWidthToFit;

    uint16_t getFitStr(String& ret_str) const;
    FieldType _type = TYPE_TEXT;
  };

}  // namespace pixeler
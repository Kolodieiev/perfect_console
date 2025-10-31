#pragma once
#pragma GCC optimize("O3")

#include <vector>

#include "./IWidget.h"

namespace pixeler
{
  class IWidgetContainer : public IWidget
  {
  public:
    IWidgetContainer(uint16_t widget_ID, IWidget::TypeID type_ID);
    virtual ~IWidgetContainer();

    /**
     * @brief Додає вказівник на віджет до контейнера.
     * Віджет повинен мати уникальний ідентифікатор для цього контейнера.
     * Ідентифікатор віджета повинен бути більшим за 0.
     *
     * @param widget_ptr Вказівник на віджет.
     */
    void addWidget(IWidget* widget_ptr);

    /**
     * @brief Видаляє віджет з контейнера і пам'яті за його ідентифікатором.
     *
     * @param widget_ID Ідентифікатор віджета.
     * @return true - у разі успіху операції.
     * @return false - інакше.
     */
    bool delWidgetByID(uint16_t widget_ID);

    /*!
     * @brief
     *       Знайти віджет у контейнері за його ідентифікатором.
     * @param widget_ID
     *       Ідентифікатор віджета.
     * @return
     *        Вказівник на віджет у разі успіху. Інакше nullptr.
     */

    /**
     * @brief Шукає у своєму контейнері віджет з указаним ідентифікатором.
     *
     * @param widget_ID Ідентифікатор віджета.
     * @return IWidget* - Вказівник на віджет у разі успіху операції.
     * @return nullptr - Інкаше.
     */
    IWidget* getWidgetByID(uint16_t widget_ID) const;

    /*!
     * @brief
     *       Отримати віджет за його порядковим номером у контейнері.
     * @param widget_indx
     *       Порядковий номер віджета.
     * @return
     *       Вказівник на віджет у разі успіху. Інакше nullptr.
     */

    /**
     * @brief Повертає вказівник на віджет за його порядковим номером у контейнері.
     *
     * @param widget_indx Порядковий номер віджета у контейнері.
     * @return IWidget* - Вказівник на віджет.
     * @return nullptr - Якщо віджет за вказаною позицією відсутній.
     */
    IWidget* getWidgetByIndx(uint16_t widget_indx) const;

    /*!
     * @brief
     *       Знайти віджет у цьому контейнері за його позицією на екрані.
     * @param x
     *       Позиція віджета по осі X.
     * @param y
     *       Позиція віджета по осі Y.
     * @return
     *       Вказівник на самий глибоко-вкладений віджет у разі успіху. Інакше вказівник на цей контейнер.
     */

    /**
     * @brief Видаляє усі віджети з контейнера та очищує пам'ять, яку вони займали.
     *
     */
    void delWidgets();

    /**
     * @brief Повертає кількість віджетів у контейнері.
     *
     * @return uint16_t
     */
    uint16_t getSize() const
    {
      return _widgets.size();
    }

    /**
     * @brief Вмикає перерисовку контейнера та його вмісту.
     *
     */
    void enable()
    {
      _is_enabled = true;
    }

    /**
     * @brief Вимикає перерисовку контейнера та його вмісту.
     *
     */
    void disable()
    {
      _is_enabled = false;
    }

    /**
     * @brief Повертає поточний стан прапору, який відповідає за перерисовку контейнера та його вмісту.
     *
     * @return true - Якщо перерисовка увімкнена.
     * @return false - Інакше.
     */
    bool isEnabled() const
    {
      return _is_enabled;
    }

  protected:
    std::vector<IWidget*> _widgets;
    bool _is_enabled{true};
  };
}  // namespace pixeler
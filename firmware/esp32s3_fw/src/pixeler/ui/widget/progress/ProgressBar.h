#pragma once
#pragma GCC optimize("O3")
#include "../IWidget.h"

namespace pixeler
{
  class ProgressBar final : public IWidget
  {
  public:
    explicit ProgressBar(uint16_t widget_ID);
    virtual ~ProgressBar();

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
     * @return ProgressBar*
     */
    virtual ProgressBar* clone(uint16_t id) const override;

    /**
     * @brief Повертає ідентифікатор типу.
     * Використовується в системі приведення типу.
     *
     * @return constexpr TypeID
     */
    static constexpr TypeID staticType()
    {
      return TypeID::TYPE_ID_PROGRESSBAR;
    }

    /**
     * @brief Встановлює максимальне значення прогресу.
     *
     * @param max
     */
    void setMax(uint16_t max);

    /**
     * @brief Встановлює поточне значення прогресу.
     *
     * @param progress
     */
    void setProgress(uint16_t progress);

    /**
     * @brief Встановлює колір фону смужки прогресу.
     *
     * @param color
     */
    void setProgressColor(uint16_t color)
    {
      _progress_color = color;
      _is_changed = true;
    }

    /**
     * @brief Встановлює орієнтацію віджета.
     * По замовчуванню вастановлено HORIZONTAL.
     *
     * @param orientation Може мати значення: VERTICAL / HORIZONTAL.
     */
    void setOrientation(Orientation orientation)
    {
      _orientation = orientation;
      _is_changed = true;
    }

    /**
     * @brief Повертає значення орієнтації віджета.
     *
     * @return Orientation
     */
    Orientation getOrientation() const
    {
      return _orientation;
    }

    /**
     * @brief Скидає значення прогресу до 1.
     *
     */
    void reset();

    /**
     * @brief Повертає поточне значення прогресу.
     *
     * @return uint16_t
     */
    uint16_t getProgress() const
    {
      return _progress;
    }

    /**
     * @brief Повертає максимально можливе значення прогресу.
     *
     * @return uint16_t
     */
    uint16_t getMax() const
    {
      return _max;
    }

  private:
    using IWidget::isTransparent;
    using IWidget::setTransparency;

  private:
    uint16_t _progress{1};
    uint16_t _max{1};
    uint16_t _progress_color{0xFFFF};
    uint16_t _prev_progress{1};
    //
    Orientation _orientation{HORIZONTAL};
    // opt
    bool _is_first_draw{true};
  };

}  // namespace pixeler
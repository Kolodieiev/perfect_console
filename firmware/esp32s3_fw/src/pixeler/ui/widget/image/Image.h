#pragma once
#pragma GCC optimize("O3")

#include <Arduino.h>

#include "../IWidget.h"

namespace pixeler
{
  class Image final : public IWidget
  {
    // TODO додати анімацію
  public:
    explicit Image(uint16_t widget_ID);
    virtual ~Image();

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
     * @return Image*
     */
    virtual Image* clone(uint16_t id) const override;

    /**
     * @brief Повертає ідентифікатор типу.
     * Використовується в системі приведення типу.
     *
     * @return constexpr TypeID
     */
    static constexpr TypeID staticType()
    {
      return TypeID::TYPE_ID_IMAGE;
    }

    /**
     * @brief Встановлює колір, з яким пікселі не будуть відображатися.
     *
     * @param color
     */
    void setTranspColor(uint16_t color);

    /**
     * @brief Видаляє прозорість у зображення.
     *
     */
    void clearTransparency();
    /**
     * @brief Встановлює вказівник на буффер із зображенням.
     * Буфер не буде видалено разом із віджетом.
     *
     * @param image_ptr Вказівник на буфер із зображенням.
     */
    void setSrc(const uint16_t* image_ptr);

  private:
    const uint16_t* _img_ptr{nullptr};
    uint16_t _transparent_color{COLOR_TRANSPARENT};

    bool _has_transp_color{false};
  };

}  // namespace pixeler
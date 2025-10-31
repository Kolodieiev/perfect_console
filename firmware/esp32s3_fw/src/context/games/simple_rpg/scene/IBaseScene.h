#pragma once
#include <stdint.h>

#include "../SceneID.h"               // Заголовковий файл, що містить ідентифіктаори сцен
#include "../obj/hero/HeroObj.h"      // Клас персонажа
#include "../obj/portal/PortalObj.h"  // Клас порталу
#include "../ui/BaseUI.h"             // Клас кастомного UI
#include "pixeler/game/IGameScene.h"

using namespace pixeler;

namespace simple_rpg
{
  class IBaseScene : public IGameScene
  {
  public:
    explicit IBaseScene(DataStream& stored_objs);
    virtual ~IBaseScene() override;
    virtual void update() override;

  protected:
    // Об'єкти за наступними вказівниками обов'язкові для кожного рівня
    // Тому їх додано в базовий клас, щоб не дублювати код
    HeroObj* _hero{nullptr};
    PortalObj* _portal{nullptr};
    BaseUI* ui;

  private:
    // Метод для створення персонажа
    void createHeroObj();
    // Метод для створення порталу, що перемикає сцени
    void createPortalObj();
    void deserialize(DataStream& stored_objs);
    //
  };
}  // namespace simple_rpg
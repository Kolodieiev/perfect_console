#pragma once
#include <stdint.h>

#include "meow/driver/graphics/GraphicsDriver.h"
#include "meowui_setup/context_id.h"

// -------------------------------- Підключи нижче заголовкові файли контекстів першого рівня
#include "exmpl_context/home/HomeContext.h"

// -------------------------------- Додай перемикання контексту за прикладом
#define SCREEN_CASES                                    \
    case ContextID::ID_CONTEXT_HOME:                    \
        context = new HomeContext();                    \
        break;                                          \
// -------------------------------- Стартовий контекст
#define START_CONTEXT HomeContext

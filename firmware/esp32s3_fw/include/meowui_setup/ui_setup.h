#pragma once
#include <stdint.h>

#include "meow/driver/graphics/GraphicsDriver.h"
#include "meowui_setup/context_id.h"

// -------------------------------- Підключи нижче заголовкові файли контекстів першого рівня
#include "context/splash/SplashContext.h"
#include "context/home/HomeContext.h"
#include "context/menu/MenuContext.h"
#include "context/mp3/Mp3Context.h"
#include "context/files/FilesContext.h"

// -------------------------------- Додай перемикання контексту за прикладом
#define SCREEN_CASES                   \
    case ContextID::ID_CONTEXT_SPLASH: \
        context = new SplashContext(); \
        break;                         \
    case ContextID::ID_CONTEXT_HOME:   \
        context = new HomeContext();   \
        break;                         \
    case ContextID::ID_CONTEXT_MENU:   \
        context = new MenuContext();   \
        break;                         \
    case ContextID::ID_CONTEXT_MP3:    \
        context = new Mp3Context();    \
        break;                         \
    case ContextID::ID_CONTEXT_FILES:  \
        context = new FilesContext();  \
        break;

// -------------------------------- Стартовий контекст
#define START_CONTEXT SplashContext

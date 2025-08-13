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
#include "context/firmware/FirmwareContext.h"
#include "context/reader/ReaderContext.h"
#include "context/pref/PrefSelectContext.h"
#include "context/games/GamesListContext.h"
#include "context/wifi/WiFiContext.h"
// Ігрові контексти
#include "context/games/sokoban/SokobanContext.h"
#include "context/games/test_server/TestServerContext.h"
#include "context/games/simple_rpg/SimpleRpgContext.h"

// -------------------------------- Додай перемикання контексту за прикладом
#define SCREEN_CASES                                    \
    case ContextID::ID_CONTEXT_SPLASH:                  \
        context = new SplashContext();                  \
        break;                                          \
    case ContextID::ID_CONTEXT_HOME:                    \
        context = new HomeContext();                    \
        break;                                          \
    case ContextID::ID_CONTEXT_MENU:                    \
        context = new MenuContext();                    \
        break;                                          \
    case ContextID::ID_CONTEXT_MP3:                     \
        context = new Mp3Context();                     \
        break;                                          \
    case ContextID::ID_CONTEXT_FILES:                   \
        context = new FilesContext();                   \
        break;                                          \
    case ContextID::ID_CONTEXT_GAMES:                   \
        context = new GamesListContext();               \
        break;                                          \
    case ContextID::ID_CONTEXT_PREF_SEL:                \
        context = new PrefSelectContext();              \
        break;                                          \
    case ContextID::ID_CONTEXT_READER:                  \
        context = new ReaderContext();                  \
        break;                                          \
    case ContextID::ID_CONTEXT_FIRMWARE:                \
        context = new FirmwareContext();                \
        break;                                          \
    case ContextID::ID_CONTEXT_SOKOBAN:                 \
        context = new sokoban::SokobanContext();        \
        break;                                          \
    case ContextID::ID_CONTEXT_TEST_SERVER:             \
        context = new test_server::TestServerContext(); \
        break;                                          \
    case ContextID::ID_CONTEXT_SIMPLE_RPG:              \
        context = new simple_rpg::SimpleRpgContext();   \
        break;                                          \
    case ContextID::ID_CONTEXT_WIFI:                    \
        context = new WiFiContext();                    \
        break;

// -------------------------------- Стартовий контекст
#define START_CONTEXT SplashContext

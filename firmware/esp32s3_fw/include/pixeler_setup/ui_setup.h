#pragma once
#include <stdint.h>

#include <functional>
#include <unordered_map>

#include "pixeler/driver/graphics/DisplayWrapper.h"
#include "pixeler_setup/context_id.h"

// -------------------------------- Підключи нижче заголовкові файли контекстів першого рівня
#include "context/files/FilesContext.h"
#include "context/firmware/FirmwareContext.h"
#include "context/games/GamesListContext.h"
#include "context/home/HomeContext.h"
#include "context/menu/MenuContext.h"
#include "context/mp3/Mp3Context.h"
#include "context/pref/PrefSelectContext.h"
#include "context/reader/ReaderContext.h"
#include "context/splash/SplashContext.h"
#include "context/wifi/WiFiContext.h"
// Ігрові контексти
#include "context/games/simple_rpg/SimpleRpgContext.h"
#include "context/games/sokoban/SokobanContext.h"
#include "context/games/test_server/TestServerContext.h"

namespace pixeler
{
  // -------------------------------- Додай перемикання контексту за прикладом
  std::unordered_map<ContextID, std::function<IContext*()>> _context_id_map = {
      {ContextID::ID_CONTEXT_SPLASH, []()
       { return new SplashContext(); }},
      {ContextID::ID_CONTEXT_HOME, []()
       { return new HomeContext(); }},
      {ContextID::ID_CONTEXT_MENU, []()
       { return new MenuContext(); }},
      {ContextID::ID_CONTEXT_MP3, []()
       { return new Mp3Context(); }},
      {ContextID::ID_CONTEXT_FILES, []()
       { return new FilesContext(); }},
      {ContextID::ID_CONTEXT_GAMES, []()
       { return new GamesListContext(); }},
      {ContextID::ID_CONTEXT_PREF_SEL, []()
       { return new PrefSelectContext(); }},
      {ContextID::ID_CONTEXT_READER, []()
       { return new ReaderContext(); }},
      {ContextID::ID_CONTEXT_FIRMWARE, []()
       { return new FirmwareContext(); }},
      {ContextID::ID_CONTEXT_SOKOBAN, []()
       { return new sokoban::SokobanContext(); }},
      {ContextID::ID_CONTEXT_TEST_SERVER, []()
       { return new test_server::TestServerContext(); }},
      {ContextID::ID_CONTEXT_SIMPLE_RPG, []()
       { return new simple_rpg::SimpleRpgContext(); }},
      {ContextID::ID_CONTEXT_WIFI, []()
       { return new WiFiContext(); }},
  };
}  // namespace pixeler

// -------------------------------- Стартовий контекст
#define START_CONTEXT SplashContext

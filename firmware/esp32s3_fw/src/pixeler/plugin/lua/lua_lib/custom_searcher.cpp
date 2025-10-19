#pragma GCC optimize("O3")
#include "custom_searcher.h"

#include <Arduino.h>
//
#include "./require/lua_i2c.h"
#include "./require/lua_net.h"
#include "./require/lua_pwm.h"
#include "./require/lua_sd.h"
#include "./require/lua_uart.h"
#include "./require/lua_wifi.h"

const char STR_ERR_NO_LOADER[] = "Відсутній завантажувач для модуля: %s";

// Додай сюди функції для підключення бібліотек з допомогою require
int custom_searcher(lua_State* L)
{
  const char* lib_name = luaL_checkstring(L, 1);
  if (strcmp(lib_name, STR_LIB_NAME_SD) == 0)
    lua_pushcfunction(L, lua_open_sd);
  else if (strcmp(lib_name, STR_LIB_NAME_WIFI) == 0)
    lua_pushcfunction(L, lua_open_wifi);
  else if (strcmp(lib_name, STR_LIB_NAME_NET) == 0)
    lua_pushcfunction(L, lua_open_net);
  else if (strcmp(lib_name, STR_LIB_NAME_UART) == 0)
    lua_pushcfunction(L, lua_open_uart);
  else if (strcmp(lib_name, STR_LIB_NAME_I2C) == 0)
    lua_pushcfunction(L, lua_open_i2c);
  else if (strcmp(lib_name, STR_LIB_NAME_PWM) == 0)
    lua_pushcfunction(L, lua_open_pwm);
  else
    luaL_error(L, STR_ERR_NO_LOADER, lib_name);

  return 1;
}

void register_custom_searcher(lua_State* L)
{
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "searchers");
  int searchers_len = lua_rawlen(L, -1);
  lua_pushinteger(L, searchers_len + 1);
  lua_pushcfunction(L, custom_searcher);
  lua_rawset(L, -3);
  lua_pop(L, 2);
}
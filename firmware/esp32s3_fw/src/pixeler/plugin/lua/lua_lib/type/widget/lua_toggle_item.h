#pragma once
#pragma GCC optimize("O3")
#include "pixeler/plugin/lua/lua.h"

const char STR_TYPE_NAME_TOGGLE_ITEM[] = "ToggleItem";

void lua_init_toggle_item(lua_State* L);

// #pragma GCC optimize("O3")
// #include "lua_toggle_item.h"
// #include "./lua_menu_item.h"
// #include "./lua_toggle_switch.h"
// #include "pixeler/plugin/lua/res/lua_strs.h"
// #include "pixeler/ui/widget/spinbox/SpinBox.h"

// using namespace pixeler;

// int lua_spin_item_new(lua_State *L)
// {
//     uint16_t id = luaL_checkinteger(L, 2);
//     ToggleItem **ret_wid_ptr = (ToggleItem **)lua_newuserdata(L, sizeof(ToggleItem *));
//     *ret_wid_ptr = new ToggleItem(id);
//     luaL_getmetatable(L, STR_TYPE_NAME_TOGGLE_ITEM);
//     lua_setmetatable(L, -2);
//     return 1;
// }

// int lua_spin_item_clone(lua_State *L)
// {
//     ToggleItem *toggle_item = *(ToggleItem **)lua_touserdata(L, 1);
//     uint16_t id = luaL_checkinteger(L, 2);
//     ToggleItem *clone = toggle_item->clone(id);

//     ToggleItem **toggle_item_clone = (ToggleItem **)lua_newuserdata(L, sizeof(ToggleItem *));
//     *toggle_item_clone = clone;

//     luaL_getmetatable(L, STR_TYPE_NAME_TOGGLE_ITEM);
//     lua_setmetatable(L, -2);
//     return 1;
// }

// int lua_toggle_item_set_spinbox(lua_State *L)
// {
//     ToggleItem *toggle_item = *(ToggleItem **)lua_touserdata(L, 1);
//     ToggleSwitch *toggle_switch = *(ToggleSwitch **)lua_touserdata(L, 2);
//     toggle_item->setToggle(toggle_switch);
//     return 0;
// }

// int lua_toggle_item_get_spinbox(lua_State *L)
// {
//     ToggleItem *toggle_item = *(ToggleItem **)lua_touserdata(L, 1);
//     ToggleSwitch **lua_toggle_switch = (ToggleSwitch **)lua_newuserdata(L, sizeof(ToggleSwitch *));
//     *lua_toggle_switch = toggle_item->getToggle();

//     luaL_getmetatable(L, STR_TYPE_NAME_TOGGLE_SWITCH);
//     lua_setmetatable(L, -2);
//     return 1;
// }

// //TODO up down setMin setMax setValue getValue setType

// const struct luaL_Reg TYPE_METH_TOGGLE_ITEM[] = {
//     {"setToggle", lua_toggle_item_set_toggle},
//     {"getToggle", lua_toggle_item_get_toggle},
//     {"on", lua_toggle_item_on},
//     {"off", lua_toggle_item_off},
//     {"isOn", lua_toggle_item_is_on},
//     {STR_LUA_WIDGET_CLONE, lua_toggle_item_clone},
//     {nullptr, nullptr},
// };

// void lua_init_spin_item(lua_State *L)
// {
//     luaL_newmetatable(L, STR_TYPE_NAME_TOGGLE_ITEM);
//     lua_newtable(L);
//     luaL_setfuncs(L, TYPE_METH_TOGGLE_ITEM, 0);
//     luaL_getmetatable(L, STR_TYPE_NAME_MENU_ITEM);
//     lua_setmetatable(L, -2);
//     lua_setfield(L, -2, STR_LUA_INDEX);
//     lua_pop(L, 1);

//     lua_newtable(L);
//     lua_pushcfunction(L, lua_toggle_item_new);
//     lua_setfield(L, -2, STR_LUA_NEW);
//     lua_setglobal(L, STR_TYPE_NAME_TOGGLE_ITEM);
// }

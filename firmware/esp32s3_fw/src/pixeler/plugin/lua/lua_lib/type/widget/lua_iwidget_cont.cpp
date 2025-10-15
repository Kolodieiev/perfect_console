#pragma GCC optimize("O3")
#include "lua_iwidget_cont.h"

#include "./lua_iwidget.h"
#include "pixeler/plugin/lua/res/lua_strs.h"
#include "pixeler/ui/widget/IWidgetContainer.h"
#include "vector"

using namespace pixeler;

void lua_push_widget_or_nil(lua_State* L, IWidget* widget)
{
  if (!widget)
  {
    lua_pushnil(L);
  }
  else
  {
    IWidget** r_widget = (IWidget**)lua_newuserdata(L, sizeof(IWidget*));
    *r_widget = widget;
    luaL_getmetatable(L, STR_TYPE_NAME_IWIDGET);
    lua_setmetatable(L, -2);
  }
}

int lua_cont_add_widget(lua_State* L)
{
  IWidgetContainer* container = *(IWidgetContainer**)lua_touserdata(L, 1);
  IWidget* widget = *(IWidget**)lua_touserdata(L, 2);
  container->addWidget(widget);
  return 0;
}

int lua_cont_delete_widget_by_id(lua_State* L)
{
  IWidgetContainer* container = *(IWidgetContainer**)lua_touserdata(L, 1);
  uint16_t id = luaL_checkinteger(L, 2);
  bool result = container->delWidgetByID(id);
  lua_pushboolean(L, result);
  return 1;
}

int lua_cont_get_widget_by_id(lua_State* L)
{
  IWidgetContainer* container = *(IWidgetContainer**)lua_touserdata(L, 1);
  uint16_t id = luaL_checkinteger(L, 2);
  IWidget* widget = container->getWidgetByID(id);
  lua_push_widget_or_nil(L, widget);
  return 1;
}

int lua_cont_get_widget_by_indx(lua_State* L)
{
  IWidgetContainer* container = *(IWidgetContainer**)lua_touserdata(L, 1);
  uint16_t indx = luaL_checkinteger(L, 2);
  IWidget* widget = container->getWidgetByIndx(indx);
  lua_push_widget_or_nil(L, widget);
  return 1;
}

int lua_cont_get_widget_by_coords(lua_State* L)
{
  IWidgetContainer* container = *(IWidgetContainer**)lua_touserdata(L, 1);
  uint16_t x = luaL_checkinteger(L, 2);
  uint16_t y = luaL_checkinteger(L, 3);
  IWidget* widget = container->getWidgetByCoords(x, y);
  lua_push_widget_or_nil(L, widget);
  return 1;
}

int lua_cont_delete_widgets(lua_State* L)
{
  IWidgetContainer* container = *(IWidgetContainer**)lua_touserdata(L, 1);
  container->delWidgets();
  return 0;
}

int lua_cont_get_size(lua_State* L)
{
  IWidgetContainer* container = *(IWidgetContainer**)lua_touserdata(L, 1);
  uint16_t size = container->getSize();
  lua_pushinteger(L, size);
  return 1;
}

int lua_cont_enable(lua_State* L)
{
  IWidgetContainer* container = *(IWidgetContainer**)lua_touserdata(L, 1);
  container->enable();
  return 0;
}

int lua_cont_disable(lua_State* L)
{
  IWidgetContainer* container = *(IWidgetContainer**)lua_touserdata(L, 1);
  container->disable();
  return 0;
}

// -------------------------------------------------------------------------------------------------------------

const struct luaL_Reg TYPE_METH_IWIDGET_CONT[] = {
    {"addWidget", lua_cont_add_widget},
    {"delWidgetByID", lua_cont_delete_widget_by_id},
    {"getWidgetByID", lua_cont_get_widget_by_id},
    {"getWidgetByIndx", lua_cont_get_widget_by_indx},
    {"getWidgetByCoords", lua_cont_get_widget_by_coords},
    {"delWidgets", lua_cont_delete_widgets},
    {"getSize", lua_cont_get_size},
    {"enable", lua_cont_enable},
    {"disable", lua_cont_disable},
    {nullptr, nullptr},
};

void lua_init_iwidget_cont(lua_State* L)
{
  luaL_newmetatable(L, STR_TYPE_NAME_IWIDGET_CONT);
  lua_newtable(L);
  luaL_setfuncs(L, TYPE_METH_IWIDGET_CONT, 0);
  luaL_getmetatable(L, STR_TYPE_NAME_IWIDGET);
  lua_setmetatable(L, -2);
  lua_setfield(L, -2, STR_LUA_INDEX);
  lua_pop(L, 1);
}

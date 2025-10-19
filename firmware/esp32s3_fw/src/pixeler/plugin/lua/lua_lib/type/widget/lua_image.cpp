#pragma GCC optimize("O3")
#include "lua_image.h"

#include "./lua_iwidget.h"
#include "pixeler/manager/ResManager.h"
#include "pixeler/plugin/lua/res/lua_strs.h"
#include "pixeler/ui/widget/image/Image.h"

using namespace pixeler;

int lua_image_new(lua_State* L)
{
  uint16_t id = luaL_checkinteger(L, 2);
  Image** ret_wid_ptr = (Image**)lua_newuserdata(L, sizeof(Image*));
  *ret_wid_ptr = new Image(id);
  luaL_getmetatable(L, STR_TYPE_NAME_IMAGE);
  lua_setmetatable(L, -2);
  return 1;
}

int lua_image_clone(lua_State* L)
{
  Image* image = *(Image**)lua_touserdata(L, 1);
  uint16_t id = luaL_checkinteger(L, 2);
  Image* clone = image->clone(id);

  Image** image_clone = (Image**)lua_newuserdata(L, sizeof(Image*));
  *image_clone = clone;

  luaL_getmetatable(L, STR_TYPE_NAME_IMAGE);
  lua_setmetatable(L, -2);

  return 1;
}

int lua_image_set_src(lua_State* L)
{
  Image* image = *(Image**)lua_touserdata(L, 1);
  uint16_t res_id = luaL_checkinteger(L, 2);

  if (res_id == 0)
    return 0;

  ImgData img_data = _res.getBmpRes(res_id);

  if (img_data.width == 0)
    return 0;

  image->setSrc(img_data.data_ptr);

  return 0;
}

const struct luaL_Reg TYPE_METH_IMAGE[] = {
    {"setSrc", lua_image_set_src},
    {STR_LUA_WIDGET_CLONE, lua_image_clone},
    {nullptr, nullptr},
};

void lua_init_image(lua_State* L)
{
  luaL_newmetatable(L, STR_TYPE_NAME_IMAGE);
  lua_newtable(L);
  luaL_setfuncs(L, TYPE_METH_IMAGE, 0);
  luaL_getmetatable(L, STR_TYPE_NAME_IWIDGET);
  lua_setmetatable(L, -2);
  lua_setfield(L, -2, STR_LUA_INDEX);
  lua_pop(L, 1);

  lua_newtable(L);
  lua_pushcfunction(L, lua_image_new);
  lua_setfield(L, -2, STR_LUA_NEW);
  lua_setglobal(L, STR_TYPE_NAME_IMAGE);
}

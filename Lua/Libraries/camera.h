#pragma once

#include <cstring>

extern "C" {
#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>
}

#include <lua5.4/lua.hpp>
#include "../../Vector2/vector2.h"
#include "../../Camera/camera.h"

static int Lua_GetCameraPosition(lua_State *L) {
    Vector2 position = Camera::GetInstance().GetPosition();
    Vector2* udata = (Vector2*)lua_newuserdata(L, sizeof(Vector2));
    *udata = position;

    luaL_getmetatable(L, "Vector2");
    lua_setmetatable(L, -2);

    return 1;
}

static const luaL_Reg cameralib[] = {
    {"GetPosition", Lua_GetCameraPosition},
    {NULL, NULL},
};

static int luaopen_camera(lua_State *L) {
    lua_newtable(L);
    luaL_setfuncs(L, cameralib, 0);
    lua_setglobal(L, "camera");
    return 0;
}
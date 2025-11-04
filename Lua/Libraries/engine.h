#pragma once

#include <lua5.4/lua.h>
#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>

// Input: Vector2 start, Vector2 end, Entity ignore?
static int Lua_EngineTraceLine(lua_State *L) {

    return 1;
}

static const luaL_Reg enginelib[] = {
    {"traceline"},
    {NULL, NULL},
};
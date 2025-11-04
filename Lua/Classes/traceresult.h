#pragma once

extern "C" {
#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>
}

#include <lua5.4/lua.hpp>

#include "../../CollisionSystem/collisionsystem.h"

static int Lua_TraceResultIndex(lua_State *L) {
    TraceResult_t *trace = (TraceResult_t*)luaL_checkudata(L, 1, "TraceResultMeta");
    const char *key = luaL_checkstring(L, 2);

    if (strcmp(key, "hit") == 0) {
        lua_pushboolean(L, trace->hit);
        return 1;
    } else if (strcmp(key, "distance") == 0) {
        lua_pushnumber(L, trace->distance);
        return 1;
    } else if (strcmp(key, "hitPoint") == 0) {
        Vector2* vec = (Vector2*)lua_newuserdata(L, sizeof(Vector2));
        *vec = trace->hitPoint; // copy the value

        // attach the existing Vector2 metatable
        luaL_getmetatable(L, "Vector2Meta");
        lua_setmetatable(L, -2);
        return 1;
    } else if (strcmp(key, "hitNormal") == 0) {
        Vector2* vec = (Vector2*)lua_newuserdata(L, sizeof(Vector2));
        *vec = trace->hitNormal; // copy the value

        // attach the existing Vector2 metatable
        luaL_getmetatable(L, "Vector2Meta");
        lua_setmetatable(L, -2);
        return 1;
    }

    // Fallback to metatable for methods
    luaL_getmetatable(L, "TraceResultMeta"); // push metatable
    lua_getfield(L, -1, key);            // push metatable[key]
    if (!lua_isnil(L, -1)) {
        return 1; // return the method
    }

    lua_pushnil(L); // key not found
    return 1;
}

static int Lua_TraceResultNewIndex(lua_State *L) {
    return 1;
}

static void Register_TraceResultClass(lua_State *L) {
    luaL_newmetatable(L, "TraceResultMeta");

    lua_pushcfunction(L, Lua_TraceResultIndex);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, Lua_TraceResultNewIndex);
    lua_setfield(L, -2, "__newindex");

    lua_pop(L, 1);
}
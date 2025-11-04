#pragma once

#include <cstring>

extern "C" {
#include <lua5.4/lauxlib.h>
#include <lua5.4/lualib.h>
}

#include <lua5.4/lua.hpp>
#include "../../Vector2/vector2.h"

/* Vector2(x or 0, y or 0) */
static int LuaCreateVector2(lua_State *L) {
    /* allocate the vector's userdata */
    Vector2 *vec = (Vector2*)lua_newuserdatauv(L, sizeof(Vector2), 0);
    vec->x = luaL_optnumber(L, 1, 0);
    vec->y = luaL_optnumber(L, 2, 0);

    luaL_getmetatable(L, "Vector2Meta");
    lua_setmetatable(L, -2); /* se the userdata and metamethods */
    return 1; /* return our new Vector2 */
}

static int LuaVector2Length(lua_State *L) {
    Vector2 *vec = (Vector2*)luaL_checkudata(L, 1, "Vector2Meta");
    lua_pushnumber(L, vec->Length());
    return 1;
}

static int LuaVector2Index(lua_State *L) {
    Vector2 *vec = (Vector2*)luaL_checkudata(L, 1, "Vector2Meta");
    const char *key = luaL_checkstring(L, 2);

    if (strcmp(key, "x") == 0) {
        lua_pushnumber(L, vec->x);
        return 1;
    }
    else if (strcmp(key, "y") == 0) {
        lua_pushnumber(L, vec->y);
        return 1;
    }

    // Fallback to metatable for methods
    luaL_getmetatable(L, "Vector2Meta"); // push metatable
    lua_getfield(L, -1, key);            // push metatable[key]
    if (!lua_isnil(L, -1)) {
        return 1; // return the method
    }

    lua_pushnil(L); // key not found
    return 1;
}

static int LuaVector2NewIndex(lua_State *L) {
    Vector2 *vec = (Vector2*)luaL_checkudata(L, 1, "Vector2Meta");
    const char *key = luaL_checkstring(L, 2);

    float value = luaL_checknumber(L, 3);

    if (strcmp(key, "x") == 0) vec->x = value;
    if (strcmp(key, "y") == 0) vec->y = value;

    return 0;
}

/* metamethod: __tostring */
static int LuaVector2ToString(lua_State *L) {
    Vector2 *vec = (Vector2*)luaL_checkudata(L, 1, "Vector2Meta");

    char buf[64];
    snprintf(buf, sizeof(buf), "x: %.2f, y: %.2f", vec->x, vec->y); // use C++ snprintf
    lua_pushstring(L, buf);
    return 1;
}

/* method :Normalize */
/* normalizes in place */
static int Lua_Vector2Normalize(lua_State *L) {
    Vector2 *vec = (Vector2*)luaL_checkudata(L, 1, "Vector2Meta");
    float len = vec->Length();

    if (len != 0) {
        vec->x /= len;
        vec->y /= len;
    }

    return 0;
}

/* method :Angle */
/* Gets the dot prouduct between 2 vectors */
// Input: Vector2 from, Vector2 to
static int Lua_Vector2Dot(lua_State *L) {
    Vector2 *from = (Vector2*)luaL_checkudata(L, 1, "Vector2Meta");
    Vector2 *to = (Vector2*)luaL_checkudata(L, 2, "Vector2Meta");
    float dot = from->Dot(*to);
    
    lua_pushnumber(L, dot);
    return 0;
}

/* example: rectange:new() ?? */
static const luaL_Reg vector2_methods[] = {
    {"Length", LuaVector2Length},
    {"Normalize", Lua_Vector2Normalize},
    {"Dot", Lua_Vector2Dot},
    {NULL, NULL}
};

/* example: __tostring */
static const luaL_Reg vector2_metamethods[] = {
    {"__tostring", LuaVector2ToString},
    {NULL, NULL}
};

static void Register_Vector2Class(lua_State *L) {
    luaL_newmetatable(L, "Vector2Meta");

    luaL_setfuncs(L, vector2_metamethods, 0);

    luaL_setfuncs(L, vector2_methods, 0);

    lua_pushcfunction(L, LuaVector2Index);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, LuaVector2NewIndex);
    lua_setfield(L, -2, "__newindex");

    lua_pop(L, 1);

    lua_register(L, "Vector2", LuaCreateVector2);
}
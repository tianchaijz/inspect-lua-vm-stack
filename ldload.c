#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "inspect.h"

static char require_push_callback_key;
static char require_pop_callback_key;
static const int sentinel_;
#define sentinel ((void *)&sentinel_)

#define require_push_callback(L, name)                                         \
  do {                                                                         \
    lua_pushlightuserdata(L, &require_push_callback_key);                      \
    lua_rawget(L, LUA_REGISTRYINDEX);                                          \
    lua_pushstring(L, name);                                                   \
    lua_call(L, 1, 0); /* run push callback */                                 \
  } while (0)
#define require_pop_callback(L, name)                                          \
  do {                                                                         \
    lua_pushlightuserdata(L, &require_pop_callback_key);                       \
    lua_rawget(L, LUA_REGISTRYINDEX);                                          \
    lua_pushstring(L, name);                                                   \
    lua_call(L, 1, 0); /* run pop callback */                                  \
  } while (0)

static int ll_require(lua_State *L) {
  const char *name = luaL_checkstring(L, 1);
  int i;

  require_push_callback(L, name);
  lua_settop(L, 1); /* _LOADED table will be at index 2 */

  lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
  lua_getfield(L, 2, name);
  if (lua_toboolean(L, -1)) { /* is it there? */
    require_pop_callback(L, name);
    if (lua_touserdata(L, -1) == sentinel) /* check loops */
      luaL_error(L, "loop or previous error loading module " LUA_QS, name);
    return 1; /* package is already loaded */
  }
  /* else must load it; iterate over available loaders */
  lua_getfield(L, LUA_ENVIRONINDEX, "loaders");
  if (!lua_istable(L, -1)) {
    require_pop_callback(L, name);
    luaL_error(L, LUA_QL("package.loaders") " must be a table");
  }
  lua_pushliteral(L, ""); /* error message accumulator */
  for (i = 1;; i++) {
    lua_rawgeti(L, -2, i); /* get a loader */
    if (lua_isnil(L, -1)) {
      require_pop_callback(L, name);
      luaL_error(L, "module " LUA_QS " not found:%s", name,
                 lua_tostring(L, -2));
    }
    lua_pushstring(L, name);
    lua_call(L, 1, 1);            /* call it */
    if (lua_isfunction(L, -1))    /* did it find module? */
      break;                      /* module loaded successfully */
    else if (lua_isstring(L, -1)) /* loader returned error message? */
      lua_concat(L, 2);           /* accumulate it */
    else
      lua_pop(L, 1);
  }

  lua_pushlightuserdata(L, sentinel);
  lua_setfield(L, 2, name);   /* _LOADED[name] = sentinel */
  lua_pushstring(L, name);    /* pass name as argument to module */
  lua_call(L, 1, 1);          /* run loaded module */
  if (!lua_isnil(L, -1))      /* non-nil return? */
    lua_setfield(L, 2, name); /* _LOADED[name] = returned value */
  lua_getfield(L, 2, name);
  if (lua_touserdata(L, -1) == sentinel) { /* module did not set a value? */
    lua_pushboolean(L, 1);                 /* use true as result */
    lua_pushvalue(L, -1);                  /* extra copy to be returned */
    lua_setfield(L, 2, name);              /* _LOADED[name] = true */
  }

  require_pop_callback(L, name);

  return 1;
}

static const luaL_Reg require_lib[] = {{"require", ll_require}, {NULL, NULL}};

static int init(lua_State *L) {
  lua_pushlightuserdata(L, &require_push_callback_key);
  lua_rawget(L, LUA_REGISTRYINDEX);
  if (lua_isfunction(L, -1)) {
    return luaL_error(L, "dload library: already inited");
  }

  lua_pop(L, 1);

  if (!lua_isfunction(L, 1))
    return luaL_error(L, "push callback of require function required");
  if (!lua_isfunction(L, 2))
    return luaL_error(L, "pop callback of require function required");

  lua_pushlightuserdata(L, &require_push_callback_key);
  lua_pushvalue(L, -3);
  lua_rawset(L, LUA_REGISTRYINDEX);

  lua_pushlightuserdata(L, &require_pop_callback_key);
  lua_pushvalue(L, -2);
  lua_rawset(L, LUA_REGISTRYINDEX);

  lua_settop(L, 0);
  lua_pushvalue(L, LUA_GLOBALSINDEX);
  luaL_register(L, NULL, require_lib); /* replace the register function */

  return 0;
}

static const struct luaL_Reg dload_lib[] = {{"init", init}, {NULL, NULL}};

int luaopen_dload(lua_State *L) {
#if LUA_VERSION_NUM < 502
  luaL_register(L, "dload", dload_lib);
#else
  luaL_newlib(L, dload_lib);
#endif

  return 1;
}

/* vi:set ft=c ts=2 sw=2 et fdm=marker: */

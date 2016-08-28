#ifndef _INSPECT_H_INCLUDED_
#define _INSPECT_H_INCLUDED_

#include <stdio.h>

#include <lauxlib.h>
#include <lua.h>
#include <luaconf.h>
#include <lualib.h>

#define LUA_INSPECT_NAME "inspect"
#define LUA_INSPECT_PATH "/tmp/inspect.lua"

#ifdef LOG_FILE
#define dd(fmt, ...)                                                           \
  do {                                                                         \
    FILE *fp = fopen(LOG_FILE, "a");                                           \
    fprintf(fp, fmt "\n", ##__VA_ARGS__);                                      \
    fclose(fp);                                                                \
  } while (0)
#else
#define dd(fmt, ...) fprintf(stdout, fmt "\n", ##__VA_ARGS__)
#endif

#define absIndex(L, i)                                                         \
  ((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : lua_gettop(L) + (i) + 1)
#define showError(L, fmt, ...)                                                 \
  do {                                                                         \
    dd(fmt, ##__VA_ARGS__);                                                    \
    luaL_error(L, fmt, ##__VA_ARGS__);                                         \
  } while (0)
#define showValue(L, i)                                                        \
  do {                                                                         \
    dd("%d: %s", (i), runInspect(L, (i)));                                     \
    lua_pop(L, 1);                                                             \
  } while (0)
#define stackDump(L)                                                           \
  do {                                                                         \
    dd("[stack dump at %s +%d]", __FILE__, __LINE__);                          \
    int i = lua_gettop(L);                                                     \
    for (; i; --i) {                                                           \
      showValue(L, i);                                                         \
    }                                                                          \
  } while (0)

static void loadError(lua_State *L, const char *filename) {
  char err[100] = {};
  snprintf(err, 100,
           "error loading module " LUA_QS " from file " LUA_QS ":\n\t%s",
           lua_tostring(L, 1), filename, lua_tostring(L, -1));
  showError(L, "%s", err);
}

static void loadInspect(lua_State *L) {
  lua_getfield(L, LUA_REGISTRYINDEX, LUA_INSPECT_NAME);
  if (!lua_isnil(L, -1))
    return; /* already loaded */

  lua_pop(L, 1);

  lua_getglobal(L, "string");
  if (lua_isnil(L, -1)) {
    luaL_openlibs(L); /* opens the standard libraries */
  }

  lua_pop(L, 1);

  if (luaL_loadfile(L, LUA_INSPECT_PATH) != 0)
    loadError(L, LUA_INSPECT_PATH);

  lua_call(L, 0, 1); /* run loaded module */

  lua_setfield(L, LUA_REGISTRYINDEX, LUA_INSPECT_NAME);
  lua_getfield(L, LUA_REGISTRYINDEX, LUA_INSPECT_NAME);

  return; /* library loaded successfully */
}

static const char *runInspect(lua_State *L, int idx) {
  idx = absIndex(L, idx);
  loadInspect(L);
  lua_pushvalue(L, idx);

  /* do the call (1 arguments, 1 result) */
  if (lua_pcall(L, 1, 1, 0) != 0)
    showError(L, "error running inspect: %s", lua_tostring(L, -1));

  return lua_tostring(L, -1);
}

#endif

/* vi:set ft=c ts=2 sw=2 et fdm=marker: */

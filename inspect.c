#include <stdio.h>

#include <lauxlib.h>
#include <lua.h>
#include <luaconf.h>
#include <lualib.h>

#define LUA_INSPECT_NAME "inspect"
#define LUA_INSPECT_PATH "/tmp/inspect.lua"

#define log(format, ...) fprintf(stdout, format "\n", ##__VA_ARGS__)
#define stackDump(L)                                                           \
  do {                                                                         \
    log("[stack dump at %s +%d]", __FILE__, __LINE__);                         \
    _stackDump(L);                                                             \
  } while (0)
#define absIndex(L, i)                                                         \
  ((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : lua_gettop(L) + (i) + 1)
#define showError(L, fmt, ...)                                                 \
  do {                                                                         \
    log(fmt, ##__VA_ARGS__);                                                   \
    luaL_error(L, fmt, ##__VA_ARGS__);                                         \
  } while (0)
#define showValue(L, i)                                                        \
  do {                                                                         \
    log("%d: %s", (i), runInspect(L, (i)));                                    \
    lua_pop(L, 1);                                                             \
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
  luaL_openlibs(L);

  if (luaL_loadfile(L, LUA_INSPECT_PATH) != 0)
    loadError(L, LUA_INSPECT_PATH);

  lua_call(L, 0, 1); /* run loaded module */

  lua_setfield(L, LUA_REGISTRYINDEX, LUA_INSPECT_NAME);
  lua_getfield(L, LUA_REGISTRYINDEX, LUA_INSPECT_NAME);

  return; /* library loaded successfully */
}

const char *runInspect(lua_State *L, int idx) {
  idx = absIndex(L, idx);
  loadInspect(L);
  lua_pushvalue(L, idx);

  /* do the call (1 arguments, 1 result) */
  if (lua_pcall(L, 1, 1, 0) != 0)
    showError(L, "error running inspect: %s", lua_tostring(L, -1));

  return lua_tostring(L, -1);
}

static void _stackDump(lua_State *L) {
  int i = lua_gettop(L);
  for (; i; --i) {
    showValue(L, i);
  }
}

static int printTable(lua_State *L) {
  stackDump(L);
  showValue(L, 1);
  return 0; /* lua vm will call lua_pop(L, 1) automatically */
}

int main(int ac, char *av[]) {
  lua_State *L = luaL_newstate(); /* init Lua-interpreter */
  luaL_openlibs(L);

  lua_register(L, "printTable",
               printTable);        /* register "printTable" function */
  luaL_loadfile(L, "script.lua");  /* load script.lua */
  lua_pcall(L, 0, LUA_MULTRET, 0); /* execute script */

  lua_close(L);
  return 0;
}

/* vi:set ft=c ts=2 sw=2 et fdm=marker: */

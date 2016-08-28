#include "inspect.h"

static int inspect_package(lua_State *L) {
  lua_getglobal(L, "package");
  showValue(L, -1);
  lua_pop(L, 1);

  return 0;
}

static int inspect_global(lua_State *L) {
  lua_pushvalue(L, LUA_GLOBALSINDEX);
  showValue(L, -1);
  lua_pop(L, 1);

  return 0;
}

static int inspect_registry(lua_State *L) {
  lua_pushvalue(L, LUA_REGISTRYINDEX);
  showValue(L, -1);
  lua_pop(L, 1);

  return 0;
}

/* as loadstring use lua_load to load the chunk */
static int inspect_loadfile(lua_State *L) {
  const char *file = luaL_checkstring(L, 1);
  luaL_loadfile(L, file);

  stackDump(L);
  /* lua "require" set the return value to true when the value is nil */
  lua_call(L, 0, LUA_MULTRET); /* execute script */
  stackDump(L);

  return 0;
}

static const struct luaL_Reg linspect_lib[] = {
    {"inspect_package", inspect_package},
    {"inspect_global", inspect_global},
    {"inspect_registry", inspect_registry},
    {"inspect_loadfile", inspect_loadfile},
    {NULL, NULL}};

int luaopen_linspect(lua_State *L) {
#if LUA_VERSION_NUM < 502
  luaL_register(L, "linspect", linspect_lib);
#else
  luaL_newlib(L, linspect_lib);
#endif

  return 1;
}

/* vi:set ft=c ts=2 sw=2 et fdm=marker: */

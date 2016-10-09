#include "inspect.h"

static int print_table(lua_State *L) {
  stackDump(L);
  showValue(L, 1);
  return 0; /* lua vm will call lua_pop(L, 1) automatically */
}

int main(int ac, char *av[]) {
  lua_State *L = luaL_newstate(); /* init Lua-interpreter */
  luaL_openlibs(L);

  /* register "print_table" function */
  lua_register(L, "print_table", print_table);

  luaL_loadfile(L, "scripts/script.lua"); /* load script.lua */
  if (lua_isstring(L, -1)) {
    dd("%s", lua_tostring(L, -1));
    return 1;
  }

  lua_pcall(L, 0, LUA_MULTRET, 0); /* execute script */

  if (lua_isstring(L, -1)) {
    dd("%s", lua_tostring(L, -1));
  }

  lua_close(L);
  return 0;
}

/* vi:set ft=c ts=2 sw=2 et fdm=marker: */

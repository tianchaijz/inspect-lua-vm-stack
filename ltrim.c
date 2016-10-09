#include <ctype.h>
#include <lauxlib.h>
#include <lua.h>
#include <stddef.h>

int trim(lua_State *L) {
  const char *front;
  const char *end;
  size_t size;

  front = luaL_checklstring(L, 1, &size);
  end = &front[size - 1];

  for (; size && isspace(*front); size--, front++)
    ;
  for (; size && isspace(*end); size--, end--)
    ;

  lua_pushlstring(L, front, (size_t)(end - front) + 1);
  return 1;
}

int luaopen_ltrim(lua_State *L) {
  lua_register(L, "trim", trim);
  return 0;
}

/* vi:set ft=c ts=2 sw=2 et fdm=marker: */

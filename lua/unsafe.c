#include <luajit-2.1/lua.h>
#include <luajit-2.1/lauxlib.h>
#include <luajit-2.1/lualib.h>
#include "nvim.h"
#define luaL_reg luaL_Reg

extern Integer nlua_pop_Integer(lua_State *lstate, Error *err);
extern String nlua_pop_String(lua_State *lstate, Error *err);
extern void nlua_pop_keydict(lua_State *L, void *retval, field_hash hashy, Error *err);
#define api_free_boolean(value)
#define api_free_integer(value)
#define api_free_float(value)
#define api_free_buffer(value)
#define api_free_string(value)
#define api_free_keydict_highlight(value)
extern void api_free_keydict(void *dict, KeySetLink *table);
extern void api_set_error(Error *err, ErrorType errType, const char *format, ...);
extern Object *KeyDict_highlight_get_field(void *retval, const char *str, size_t len);
extern void nvim_set_hl(Integer ns_id, String name, Dict(highlight) *val, Error *err);

static int print(lua_State *L) {
	Error err = ERROR_INIT;
	int a = nlua_pop_Integer(L, &err);
	lua_getglobal(L, "print");
	lua_pushnumber(L, a);
	lua_call(L, 1, 1);
	int sum = (int)lua_tointeger(L, -1);
	lua_pop(L, 1);
	return 0;
}

static int nlua_api_nvim_set_hl(lua_State *lstate)
{
  Error err = ERROR_INIT;
  // if (lua_gettop(lstate) != 3) {
  //   api_set_error(&err, kErrorTypeValidation, "Expected 3 arguments");
  //   goto exit_0;
  // }
  // if (!nlua_is_deferred_safe()) {
  //   return luaL_error(lstate, e_luv_api_disabled, "nvim_set_hl");
  // }
	KeyDict_highlight arg3 = { 0 }; nlua_pop_keydict(lstate, &arg3, KeyDict_highlight_get_field, &err);
	const String arg2 = nlua_pop_String(lstate, &err);
	const Integer arg1 = nlua_pop_Integer(lstate, &err);

  nvim_set_hl(arg1, arg2, &arg3, &err);

  api_free_integer(arg1);
exit_2:
  api_free_string(arg2);
exit_1:
  api_free_keydict_highlight(&arg3);

// exit_0:
//   if (ERROR_SET(&err)) {
//     luaL_where(lstate, 1);
//     lua_pushstring(lstate, err.msg);
//     api_clear_error(&err);
//     lua_concat(lstate, 2);
//     return lua_error(lstate);
//   }

  return 0;
}

static const struct luaL_reg unsafe[] = {
  {"print", print},
	{"nvim_set_hl", nlua_api_nvim_set_hl},
	{NULL, NULL}
};

int luaopen_unsafe(lua_State *L) {
	luaL_openlib(L, "unsafe", unsafe, 0);
	return 1;
}

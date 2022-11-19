#include <luajit-2.1/lua.h>
#include <luajit-2.1/lauxlib.h>
#include <luajit-2.1/lualib.h>
#include "nvim.h"
#include <assert.h>
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
#define api_clear_error(value)
extern void free (void *__ptr) __THROW;
extern void *malloc(size_t size);
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

static int nlua_api_nvim_set_hl(lua_State *lstate) {
  Error err = ERROR_INIT;
  // if (lua_gettop(lstate) != 3) {
  //   api_set_error(&err, kErrorTypeValidation, "Expected 3 arguments");
  //   goto exit_0;
  // }
  // if (!nlua_is_deferred_safe()) {
  //   return luaL_error(lstate, e_luv_api_disabled, "nvim_set_hl");
  // }
	KeyDict_highlight arg3 = { 0 };
	nlua_pop_keydict(lstate, &arg3, KeyDict_highlight_get_field, &err);
	const String arg2 = nlua_pop_String(lstate, &err);
	const Integer arg1 = nlua_pop_Integer(lstate, &err);

  nvim_set_hl(arg1, arg2, &arg3, &err);

  api_free_integer(arg1);
exit_2:
  api_free_string(arg2);
exit_1:
  api_free_keydict_highlight(&arg3);

exit_0:
  if (ERROR_SET(&err)) {
    luaL_where(lstate, 1);
    lua_pushstring(lstate, err.msg);
    api_clear_error(&err);
    lua_concat(lstate, 2);
    return lua_error(lstate);
  }

  return 0;
}

typedef long long mpack_sintmax_t;
typedef unsigned long long mpack_uintmax_t;

typedef union {
  void *p;
  mpack_uintmax_t u;
  mpack_sintmax_t i;
  double d;
} mpack_data_t;

typedef enum {
  MPACK_TOKEN_NIL       = 1,
  MPACK_TOKEN_BOOLEAN   = 2,
  MPACK_TOKEN_UINT      = 3,
  MPACK_TOKEN_SINT      = 4,
  MPACK_TOKEN_FLOAT     = 5,
  MPACK_TOKEN_CHUNK     = 6,
  MPACK_TOKEN_ARRAY     = 7,
  MPACK_TOKEN_MAP       = 8,
  MPACK_TOKEN_BIN       = 9,
  MPACK_TOKEN_STR       = 10,
  MPACK_TOKEN_EXT       = 11
} mpack_token_type_t;
typedef unsigned int mpack_uint32_t;

typedef struct mpack_value_s {
  mpack_uint32_t lo, hi;
} mpack_value_t;

typedef struct mpack_token_s {
  mpack_token_type_t type;  /* Type of token */
  mpack_uint32_t length;    /* Byte length for str/bin/ext/chunk/float/int/uint.
                               Item count for array/map. */
  union {
    mpack_value_t value;    /* 32-bit parts of primitives (bool,int,float) */
    const char *chunk_ptr;  /* Chunk of data from str/bin/ext */
    int ext_type;           /* Type field for ext tokens */
  } data;
} mpack_token_t;

typedef struct mpack_node_s {
  mpack_token_t tok;
  size_t pos;
  /* flag to determine if the key was visited when traversing a map */
  int key_visited;
  /* allow 2 instances mpack_data_t per node. the reason is that when
   * serializing, the user may need to keep track of traversal state besides the
   * parent node reference */
  mpack_data_t data[2];
} mpack_node_t;
#define MPACK_MAX_TOKEN_LEN 9  /* 64-bit ints/floats plus type code */
typedef struct mpack_tokbuf_s {
  char pending[MPACK_MAX_TOKEN_LEN];
  mpack_token_t pending_tok;
  size_t ppos, plen;
  mpack_uint32_t passthrough;
} mpack_tokbuf_t;
#define MPACK_PARSER_STRUCT(c)      \
  struct {                          \
    mpack_data_t data;              \
    mpack_uint32_t size, capacity;  \
    int status;                     \
    int exiting;                    \
    mpack_tokbuf_t tokbuf;          \
    mpack_node_t items[c + 1];      \
  }
#ifndef MPACK_MAX_OBJECT_DEPTH
# define MPACK_MAX_OBJECT_DEPTH 32
#endif
typedef MPACK_PARSER_STRUCT(MPACK_MAX_OBJECT_DEPTH) mpack_parser_t;

typedef struct {
  lua_State *L;
  mpack_parser_t *parser;
  int reg, ext, unpacking, mtdict;
  char *string_buffer;
} Unpacker;

typedef struct {
  lua_State *L;
  mpack_parser_t *parser;
  int reg, ext, root, packing, mtdict;
  int is_bin, is_bin_fn;
} Packer;

extern void mpack_parser_init(mpack_parser_t *p, mpack_uint32_t c);
static int lmpack_ref(lua_State *L, int reg)
{
#ifdef MPACK_DEBUG_REGISTRY_LEAK
  return luaL_ref(L, LUA_REGISTRYINDEX);
#else
  int rv;
  lua_rawgeti(L, LUA_REGISTRYINDEX, reg);
  lua_pushvalue(L, -2);
  rv = luaL_ref(L, -2);
  lua_pop(L, 2);
  return rv;
#endif
}
typedef void(*mpack_walk_cb)(mpack_parser_t *w, mpack_node_t *n);
extern int mpack_parse(mpack_parser_t *parser, const char **b, size_t *bl,
    mpack_walk_cb enter_cb, mpack_walk_cb exit_cb);
static void lmpack_unref(lua_State *L, int reg, int ref)
{
#ifdef MPACK_DEBUG_REGISTRY_LEAK
  luaL_unref(L, LUA_REGISTRYINDEX, ref);
#else
  lua_rawgeti(L, LUA_REGISTRYINDEX, reg);
  luaL_unref(L, -1, ref);
  lua_pop(L, 1);
#endif
}
#define NIL_NAME "mpack.NIL"
static void lmpack_pushnil(lua_State *L)
{
  lua_getfield(L, LUA_REGISTRYINDEX, NIL_NAME);
}
# define MPACK_API extern
MPACK_API bool mpack_unpack_boolean(mpack_token_t t)
{
  return t.data.value.lo || t.data.value.hi;
}
static int mpack_fits_single(double v)
{
  return (float)v == v;
}
static int mpack_is_be(void)
{
  union {
    mpack_uint32_t i;
    char c[sizeof(mpack_uint32_t)];
  } test;

  test.i = 1;
  return test.c[0] == 0;
}
#define MPACK_SWAP_VALUE(val)                                  \
  do {                                                         \
    mpack_uint32_t lo = val.lo;                                \
    val.lo = val.hi;                                           \
    val.hi = lo;                                               \
  } while (0)
MPACK_API mpack_token_t mpack_pack_float_fast(double v)
{
  /* ieee754 single-precision limits to determine if "v" can be fully
   * represented in 4 bytes */
  mpack_token_t rv;

  if (mpack_fits_single(v)) {
    union {
      float f;
      mpack_uint32_t m;
    } conv;
    conv.f = (float)v;
    rv.length = 4;
    rv.data.value.lo = conv.m;
    rv.data.value.hi = 0;
  } else {
    union {
      double d;
      mpack_value_t m;
    } conv;
    conv.d = v;
    rv.length = 8;
    rv.data.value = conv.m;
    if (mpack_is_be()) {
      MPACK_SWAP_VALUE(rv.data.value);
    }
  }

  rv.type = MPACK_TOKEN_FLOAT;
  return rv;
}
#ifndef mpack_unpack_float
# define mpack_unpack_float mpack_unpack_float_fast
#endif
#ifndef mpack_unpack_float
# define mpack_unpack_float mpack_unpack_float_fast
#endif
MPACK_API double mpack_unpack_float_fast(mpack_token_t t)
{
  if (t.length == 4) {
    union {
      float f;
      mpack_uint32_t m;
    } conv;
    conv.m = t.data.value.lo;
    return conv.f;
  } else {
    union {
      double d;
      mpack_value_t m;
    } conv;
    conv.m = t.data.value;
    
    if (mpack_is_be()) {
      MPACK_SWAP_VALUE(conv.m);
    }

    return conv.d;
  }
}
#define POW2(n) \
  ((double)(1 << (n / 2)) * (double)(1 << (n / 2)) * (double)(1 << (n % 2)))
MPACK_API double mpack_unpack_number(mpack_token_t t)
{
  double rv;
  mpack_uint32_t hi, lo;
  if (t.type == MPACK_TOKEN_FLOAT) return mpack_unpack_float(t);
  assert(t.type == MPACK_TOKEN_UINT || t.type == MPACK_TOKEN_SINT);
  hi = t.data.value.hi;
  lo = t.data.value.lo;
  if (t.type == MPACK_TOKEN_SINT) {
    /* same idea as mpack_unpack_sint, except here we shouldn't rely on
     * mpack_uintmax_t having 64-bits, operating on the 32-bit words separately.
     */
    if (!hi) {
      assert(t.length <= 4);
      lo = (~lo & (((mpack_uint32_t)1 << ((t.length * 8) - 1)) - 1));
    } else {
      hi = ~hi;
      lo = ~lo;
    }
    lo++;
    if (!lo) hi++;
  }
  rv = (double)lo + POW2(32) * hi;
  return t.type == MPACK_TOKEN_SINT ? -rv : rv;
}
#define MPACK_PARENT_NODE(n) (((n) - 1)->pos == (size_t)-1 ? NULL : (n) - 1)
static void lmpack_parse_enter(mpack_parser_t *parser, mpack_node_t *node)
{
  Unpacker *unpacker = parser->data.p;
  lua_State *L = unpacker->L;

  switch (node->tok.type) {
    case MPACK_TOKEN_NIL:
      lmpack_pushnil(L); break;
    case MPACK_TOKEN_BOOLEAN:
      lua_pushboolean(L, (int)mpack_unpack_boolean(node->tok)); break;
    case MPACK_TOKEN_UINT:
    case MPACK_TOKEN_SINT:
    case MPACK_TOKEN_FLOAT:
      lua_pushnumber(L, mpack_unpack_number(node->tok)); break;
    case MPACK_TOKEN_CHUNK:
      assert(unpacker->string_buffer);
      memcpy(unpacker->string_buffer + MPACK_PARENT_NODE(node)->pos,
          node->tok.data.chunk_ptr, node->tok.length);
      break;
    case MPACK_TOKEN_BIN:
    case MPACK_TOKEN_STR:
    case MPACK_TOKEN_EXT:
      unpacker->string_buffer = malloc(node->tok.length);
      if (!unpacker->string_buffer) luaL_error(L, "Failed to allocate memory");
      break;
    case MPACK_TOKEN_ARRAY:
    case MPACK_TOKEN_MAP:
      lua_newtable(L);
      node->data[0].i = lmpack_ref(L, unpacker->reg);
      break;
  }
}
static void lmpack_geti(lua_State *L, int reg, int ref)
{
#ifdef MPACK_DEBUG_REGISTRY_LEAK
  lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
#else
  lua_rawgeti(L, LUA_REGISTRYINDEX, reg);
  lua_rawgeti(L, -1, ref);
  lua_replace(L, -2);
#endif
}
static void lmpack_parse_exit(mpack_parser_t *parser, mpack_node_t *node)
{
  Unpacker *unpacker = parser->data.p;
  lua_State *L = unpacker->L;
  mpack_node_t *parent = MPACK_PARENT_NODE(node);

  switch (node->tok.type) {
    case MPACK_TOKEN_BIN:
    case MPACK_TOKEN_STR:
    case MPACK_TOKEN_EXT:
      lua_pushlstring(L, unpacker->string_buffer, node->tok.length);
      free(unpacker->string_buffer);
      unpacker->string_buffer = NULL;
      if (node->tok.type == MPACK_TOKEN_EXT && unpacker->ext != LUA_NOREF) {
        /* check if there's a handler for this type */
        lmpack_geti(L, unpacker->reg, unpacker->ext);
        lua_rawgeti(L, -1, node->tok.data.ext_type);
        if (lua_isfunction(L, -1)) {
          /* stack:
           *
           * -1: ext unpacker function
           * -2: ext unpackers table 
           * -3: ext string 
           *
           * We want to call the ext unpacker function with the type and string
           * as arguments, so push those now
           */
          lua_pushinteger(L, node->tok.data.ext_type);
          lua_pushvalue(L, -4);
          lua_call(L, 2, 1);
          /* stack:
           *
           * -1: returned object
           * -2: ext unpackers table
           * -3: ext string 
           */
          lua_replace(L, -3);
        } else {
          /* the last lua_rawgeti should have pushed nil on the stack,
           * remove it */
          lua_pop(L, 1);
        }
        /* pop the ext unpackers table */
        lua_pop(L, 1);
      }
      break;
    case MPACK_TOKEN_ARRAY:
    case MPACK_TOKEN_MAP:
      lmpack_geti(L, unpacker->reg, (int)node->data[0].i);
      lmpack_unref(L, unpacker->reg, (int)node->data[0].i);
      if (node->key_visited == 0 && node->tok.type == MPACK_TOKEN_MAP) {
        lmpack_geti(L, unpacker->reg, unpacker->mtdict); // [table, mtdict]
        lua_setmetatable(L, -2); // [table]
      }

      break;
    default:
      break;
  }

  if (parent && parent->tok.type < MPACK_TOKEN_BIN) {
    /* At this point the parsed object is on the stack. Add it to the parent
     * container. First put the container on the stack. */
    lmpack_geti(L, unpacker->reg, (int)parent->data[0].i);

    if (parent->tok.type == MPACK_TOKEN_ARRAY) {
      /* Array, save the value on key equal to `parent->pos` */
      lua_pushnumber(L, (lua_Number)parent->pos);
      lua_pushvalue(L, -3);
      lua_settable(L, -3);
    } else {
      // assert(parent->tok.type == MPACK_TOKEN_MAP);
      if (parent->key_visited) {
        /* save the key on the registry */ 
        lua_pushvalue(L, -2);
        parent->data[1].i = lmpack_ref(L, unpacker->reg);
      } else {
        /* set the key/value pair */
        lmpack_geti(L, unpacker->reg, (int)parent->data[1].i);
        lmpack_unref(L, unpacker->reg, (int)parent->data[1].i);
        lua_pushvalue(L, -3);
        lua_settable(L, -3);
      }
    }
    lua_pop(L, 2);  /* pop the container/object */
  }
}

#define EMPTY_DICT_NAME "mpack.empty_dict"
enum {
  MPACK_OK = 0,
  MPACK_EOF = 1,
  MPACK_ERROR = 2
};
enum {
  MPACK_EXCEPTION = -1,
  MPACK_NOMEM = MPACK_ERROR + 1
};

static int lmpack_unpack(lua_State *L)
{
  int result;
  size_t len;
  const char *str;
  Unpacker unpacker;
  mpack_parser_t parser;

  if (lua_gettop(L) != 1)
    return luaL_error(L, "expecting exactly 1 argument"); 

  str = luaL_checklstring(L, 1, &len);

  /* initialize unpacker */
  lua_newtable(L);
  unpacker.reg = luaL_ref(L, LUA_REGISTRYINDEX);
  unpacker.ext = LUA_NOREF;
  unpacker.parser = &parser;
  mpack_parser_init(unpacker.parser, 0);
  unpacker.parser->data.p = &unpacker;
  unpacker.string_buffer = NULL;
  unpacker.L = L;

  lua_getfield(L, LUA_REGISTRYINDEX, EMPTY_DICT_NAME);
  unpacker.mtdict = lmpack_ref(L, unpacker.reg);

  result = mpack_parse(&parser, &str, &len, lmpack_parse_enter,
      lmpack_parse_exit);

  luaL_unref(L, LUA_REGISTRYINDEX, unpacker.reg);

  if (result == MPACK_NOMEM)
    return luaL_error(L, "object was too deep to unpack");
  else if (result == MPACK_EOF)
    return luaL_error(L, "incomplete msgpack string");
  else if (result == MPACK_ERROR)
    return luaL_error(L, "invalid msgpack string");
  else if (result == MPACK_OK && len)
    return luaL_error(L, "trailing data in msgpack string");

  assert(result == MPACK_OK);
  return 1;
}

static const struct luaL_reg unsafe[] = {
  {"print", print},
	{"nvim_set_hl", nlua_api_nvim_set_hl},
	{"decode", lmpack_unpack},
	{NULL, NULL}
};

int luaopen_unsafe(lua_State *L) {
	luaL_openlib(L, "unsafe", unsafe, 0);
	return 1;
}

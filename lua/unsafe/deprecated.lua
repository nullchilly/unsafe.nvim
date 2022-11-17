local ffi = require("ffi")

ffi.cdef([[
typedef bool Boolean;
typedef int64_t Integer;
typedef double Float;
typedef struct {
  char *data;
  size_t size;
} String;
typedef struct object Object;
typedef struct {
	size_t size;
	size_t capacity;
	Object *items;
} Array;

struct key_value_pair {
  String key;
  Object value;
};
typedef struct key_value_pair KeyValuePair;

typedef struct {
	size_t size;
	size_t capacity;
	KeyValuePair *items;
} Dictionary;

typedef struct {
  char *data;
  size_t size;
} String;
typedef enum {
  kErrorTypeNone = -1,
  kErrorTypeException,
  kErrorTypeValidation,
} ErrorType;
typedef struct {
  ErrorType type;
  char *msg;
} Error;
typedef enum {
  kObjectTypeNil = 0,
  kObjectTypeBoolean,
  kObjectTypeInteger,
  kObjectTypeFloat,
  kObjectTypeString,
  kObjectTypeArray,
  kObjectTypeDictionary,
  kObjectTypeLuaRef,
  // EXT types, cannot be split or reordered, see #EXT_OBJECT_TYPE_SHIFT
  kObjectTypeBuffer,
  kObjectTypeWindow,
  kObjectTypeTabpage,
} ObjectType;
struct object {
  ObjectType type;
  union {
    Boolean boolean;
    Integer integer;
    Float floating;
    String string;
    Array array;
    Dictionary dictionary;
    LuaRef luarefLuaRef;
  } data;
};
typedef struct object Object;
typedef struct {
	Object bold;
	Object standout;
	Object strikethrough;
	Object underline;
	Object undercurl;
	Object underdouble;
	Object underdotted;
	Object underdashed;
	Object italic;
	Object reverse;
	Object nocombine;
	Object default;
	Object cterm;
	Object foreground;
	Object fg;
	Object background
	Object bg;
	Object ctermfg;
	Object ctermbg;
	Object special;
	Object sp;
	Object link;
	Object global_link;
	Object fallback;
	Object blend;
} KeyDict_highlight;
void nvim_set_hl(Integer ns_id, String name, KeyDict_highlight *val, Error *err);
]])

local filename = "/home/nullchilly/code/git/catppuccin/README.md"
local ns_id = ffi.new("Integer", 0)
ffi.C.nvim_set_hl(ns_id)
print(ffi.string(c_str))

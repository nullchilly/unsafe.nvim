// https://github.com/neovim/neovim/blob/master/src/nvim/api/private/defs.h
#ifndef NVIM_H
#define NVIM_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

// #include "nvim/func_attr.h"
// #include "nvim/types.h"

#define ARRAY_DICT_INIT KV_INITIAL_VALUE
#define STRING_INIT { .data = NULL, .size = 0 }
#define OBJECT_INIT { .type = kObjectTypeNil }
#define ERROR_INIT { .type = kErrorTypeNone, .msg = NULL }
#define REMOTE_TYPE(type) typedef handle_T type

#define ERROR_SET(e) ((e)->type != kErrorTypeNone)

#ifdef INCLUDE_GENERATED_DECLARATIONS
# define ArrayOf(...) Array
# define DictionaryOf(...) Dictionary
# define Dict(name) KeyDict_##name
#endif

// #include "klib/kvec.h"
#define kvec_t(type) \
  struct { \
    size_t size; \
    size_t capacity; \
    type *items; \
  }

// Basic types
typedef enum {
  kErrorTypeNone = -1,
  kErrorTypeException,
  kErrorTypeValidation,
} ErrorType;

typedef enum {
  kMessageTypeUnknown = -1,
  // Per msgpack-rpc spec.
  kMessageTypeRequest = 0,
  kMessageTypeResponse = 1,
  kMessageTypeNotification = 2,
  kMessageTypeRedrawEvent = 3,
} MessageType;

/// Mask for all internal calls
#define INTERNAL_CALL_MASK (((uint64_t)1) << (sizeof(uint64_t) * 8 - 1))

/// Internal call from VimL code
#define VIML_INTERNAL_CALL INTERNAL_CALL_MASK

/// Internal call from lua code
#define LUA_INTERNAL_CALL (VIML_INTERNAL_CALL + 1)


typedef struct {
  ErrorType type;
  char *msg;
} Error;

typedef bool Boolean;
typedef int64_t Integer;
typedef double Float;

/// Maximum value of an Integer
#define API_INTEGER_MAX INT64_MAX

/// Minimum value of an Integer
#define API_INTEGER_MIN INT64_MIN

typedef struct {
  char *data;
  size_t size;
} String;

typedef struct object Object;
typedef kvec_t(Object) Array;

typedef struct key_value_pair KeyValuePair;
typedef kvec_t(KeyValuePair) Dictionary;

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

typedef int LuaRef;
struct object {
  ObjectType type;
  union {
    Boolean boolean;
    Integer integer;
    Float floating;
    String string;
    Array array;
    Dictionary dictionary;
    LuaRef luaref;
  } data;
};

struct key_value_pair {
  String key;
  Object value;
};

typedef Object *(*field_hash)(void *retval, const char *str, size_t len);
typedef struct {
  char *str;
  size_t ptr_off;
} KeySetLink;

#ifdef INCLUDE_GENERATED_DECLARATIONS
# include "keysets_defs.generated.h"
#endif

#endif  // NVIM_API_PRIVATE_DEFS_H

typedef int32_t RgbValue;
typedef struct attr_entry {
  int16_t rgb_ae_attr, cterm_ae_attr;  ///< HlAttrFlags
  RgbValue rgb_fg_color, rgb_bg_color, rgb_sp_color;
  int cterm_fg_color, cterm_bg_color;
  int hl_blend;
} HlAttrs;

# define Dict(name) KeyDict_##name
typedef struct {
  Object bg;
  Object fg;
  Object sp;
  Object bold;
  Object link;
  Object blend;
  Object cterm;
  Object italic;
  Object special;
  Object ctermbg;
  Object ctermfg;
  Object default_;
  Object reverse;
  Object fallback;
  Object standout;
  Object nocombine;
  Object undercurl;
  Object underline;
  Object background;
  Object foreground;
  Object global_link;
  Object underdashed;
  Object underdotted;
  Object underdouble;
  Object strikethrough;
} KeyDict_highlight;

extern HlAttrs dict2hlattrs(Dict(highlight) *dict, bool use_rgb, int *link_id, Error *err);
extern void nvim_set_hl(Integer ns_id, String name, Dict(highlight) *val, Error *err);

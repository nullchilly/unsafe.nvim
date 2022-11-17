// https://github.com/neovim/neovim/blob/master/src/nvim/api/private/defs.h

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

typedef struct {
  ErrorType type;
  char *msg;
} Error;

typedef bool Boolean;
typedef long long Integer;
typedef double Float;

typedef struct {
  char *data;
  int size;
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
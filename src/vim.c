// https://github.com/neovim/neovim/blob/master/src/nvim/api/vim.c

#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "nvim.h"
#include "lua.h"

// #include "nvim/api/buffer.h"
// #include "nvim/api/deprecated.h"
// #include "nvim/api/private/converter.h"
// #include "nvim/api/private/defs.h"
// #include "nvim/api/private/dispatch.h"
// #include "nvim/api/private/helpers.h"
// #include "nvim/api/vim.h"
// #include "nvim/api/window.h"
// #include "nvim/ascii.h"
// #include "nvim/buffer.h"
// #include "nvim/buffer_defs.h"
// #include "nvim/charset.h"
// #include "nvim/context.h"
// #include "nvim/decoration.h"
// #include "nvim/decoration_provider.h"
// #include "nvim/drawscreen.h"
// #include "nvim/edit.h"
// #include "nvim/eval.h"
// #include "nvim/eval/typval.h"
// #include "nvim/eval/userfunc.h"
// #include "nvim/ex_cmds_defs.h"
// #include "nvim/ex_docmd.h"
// #include "nvim/ex_eval.h"
// #include "nvim/file_search.h"
// #include "nvim/fileio.h"
// #include "nvim/getchar.h"
// #include "nvim/globals.h"
// #include "nvim/grid.h"
// #include "nvim/highlight.h"
// #include "nvim/highlight_defs.h"
// #include "nvim/highlight_group.h"
// #include "nvim/insexpand.h"
// #include "nvim/lua/executor.h"
// #include "nvim/mapping.h"
// #include "nvim/mark.h"
// #include "nvim/memline.h"
// #include "nvim/memory.h"
// #include "nvim/message.h"
// #include "nvim/move.h"
// #include "nvim/msgpack_rpc/channel.h"
// #include "nvim/msgpack_rpc/helpers.h"
// #include "nvim/msgpack_rpc/unpacker.h"
// #include "nvim/ops.h"
// #include "nvim/option.h"
// #include "nvim/optionstr.h"
// #include "nvim/os/input.h"
// #include "nvim/os/process.h"
// #include "nvim/popupmenu.h"
// #include "nvim/runtime.h"
// #include "nvim/state.h"
// #include "nvim/statusline.h"
// #include "nvim/types.h"
// #include "nvim/ui.h"
// #include "nvim/vim.h"
// #include "nvim/viml/parser/expressions.h"
// #include "nvim/viml/parser/parser.h"
// #include "nvim/window.h"

/// Sets a highlight group.
///
/// @note Unlike the `:highlight` command which can update a highlight group,
///       this function completely replaces the definition. For example:
///       ``nvim_set_hl(0, 'Visual', {})`` will clear the highlight group
///       'Visual'.
///
/// @note The fg and bg keys also accept the string values `"fg"` or `"bg"`
///       which act as aliases to the corresponding foreground and background
///       values of the Normal group. If the Normal group has not been defined,
///       using these values results in an error.
///
/// @param ns_id Namespace id for this highlight |nvim_create_namespace()|.
///              Use 0 to set a highlight group globally |:highlight|.
/// @param name  Highlight group name, e.g. "ErrorMsg"
/// @param val   Highlight definition map, accepts the following keys:
///                - fg (or foreground): color name or "#RRGGBB", see note.
///                - bg (or background): color name or "#RRGGBB", see note.
///                - sp (or special): color name or "#RRGGBB"
///                - blend: integer between 0 and 100
///                - bold: boolean
///                - standout: boolean
///                - underline: boolean
///                - undercurl: boolean
///                - underdouble: boolean
///                - underdotted: boolean
///                - underdashed: boolean
///                - strikethrough: boolean
///                - italic: boolean
///                - reverse: boolean
///                - nocombine: boolean
///                - link: name of another highlight group to link to, see |:hi-link|.
///                - default: Don't override existing definition |:hi-default|
///                - ctermfg: Sets foreground of cterm color |ctermfg|
///                - ctermbg: Sets background of cterm color |ctermbg|
///                - cterm: cterm attribute map, like |highlight-args|. If not set,
///                         cterm attributes will match those from the attribute map
///                         documented above.
/// @param[out] err Error details, if any
///
// TODO(bfredl): val should take update vs reset flag
HlAttrs dict2hlattrs(Dict(highlight) *dict, bool use_rgb, int *link_id, Error *err);
void nvim_set_hl(Integer ns_id, String name, Dict(highlight) *val, Error *err);

  // int hl_id = syn_check_group(name.data, name.size);
  // if (hl_id == 0) {
  //   api_set_error(err, kErrorTypeException, "Invalid highlight name: %s", name.data);
  //   return;
  // }
  // int link_id = -1;

  // HlAttrs attrs = dict2hlattrs(val, true, &link_id, err);

import ctypes
from shared import lib as so

from symbols.buffer import buffer_p

from symbols.hook import hook_p

class win_s(ctypes.Structure):
    pass

win_p = ctypes.POINTER(win_s)

class dir:
    up    = 0
    down  = 1
    left  = 2
    right = 3

root = ctypes.cast(so.win_root, ctypes.POINTER(win_p)).contents

on_split  = ctypes.cast(so.win_on_split,  hook_p)
on_delete_pre  = ctypes.cast(so.win_on_delete_pre,  hook_p)
on_delete_post = ctypes.cast(so.win_on_delete_post, hook_p)
on_create = ctypes.cast(so.win_on_create, hook_p)
on_select = ctypes.cast(so.win_on_select, hook_p)
on_buffer_set = ctypes.cast(so.win_on_buffer_set, hook_p)

initsys = so.win_initsys
initsys.argtypes = []
initsys.restype  = ctypes.c_int

killsys = so.win_killsys
killsys.argtypes = []
killsys.restype  = ctypes.c_int

split = so.win_split
split.argtypes = [win_p, ctypes.c_int]
split.restype  = win_p

delete = so.win_delete
delete.argtypes = [win_p]
delete.restype  = ctypes.c_int

get_parent = so.win_get_parent
get_parent.argtypes = [win_p]
get_parent.restype  = win_p

get_buffer = so.win_get_buffer
get_buffer.argtypes = [win_p]
get_buffer.restype  = buffer_p

set_buffer = so.win_set_buffer
set_buffer.argtypes = [win_p, buffer_p]
set_buffer.restype  = ctypes.c_int

type_isleaf = so.win_type_isleaf
type_isleaf.argtypes = [win_p]
type_isleaf.restype  = ctypes.c_int

type_issplitter = so.win_type_issplitter
type_issplitter.argtypes = [win_p]
type_issplitter.restype  = ctypes.c_int

get_offsetx = so.win_get_offsetx
get_offsetx.argtypes = [win_p]
get_offsetx.restype  = ctypes.c_ulong

get_offsety = so.win_get_offsety
get_offsety.argtypes = [win_p]
get_offsety.restype  = ctypes.c_ulong

set_offsetx = so.win_set_offsetx
set_offsetx.argtypes = [win_p, ctypes.c_ulong]
set_offsetx.restype  = ctypes.c_int

set_offsety = so.win_set_offsety
set_offsety.argtypes = [win_p, ctypes.c_ulong]
set_offsety.restype  = ctypes.c_int

on_offsetx_set = ctypes.cast(so.win_on_offsetx_set, hook_p)
on_offsety_set = ctypes.cast(so.win_on_offsety_set, hook_p)

class size:
    set_root = so.win_size_set_root
    set_root.argtypes = [ctypes.c_uint, ctypes.c_uint]
    set_root.restype  = ctypes.c_int

    adj_splitter = so.win_size_adj_splitter
    adj_splitter.argtypes = [win_p, ctypes.c_int]
    adj_splitter.restype  = ctypes.c_int

    get_x = so.win_size_get_x
    get_x.argtypes = [win_p]
    get_x.restype  = ctypes.c_uint

    get_y = so.win_size_get_y
    get_y.argtypes = [win_p]
    get_y.restype  = ctypes.c_uint

    on_adj_pre  = ctypes.cast(so.win_size_on_adj_pre, hook_p)
    on_adj_post = ctypes.cast(so.win_size_on_adj_post, hook_p)

class pos:
    get_x = so.win_pos_get_x
    get_x.argtypes = [win_p]
    get_x.restype  = ctypes.c_int

    get_y = so.win_pos_get_y
    get_y.argtypes = [win_p]
    get_y.restype  = ctypes.c_int

class select:
    set = so.win_select_set
    set.argtypes = [win_p]
    set.restype  = ctypes.c_int

    get = so.win_select_get
    get.argtypes = []
    get.restype  = win_p

class label:
    caption_set = so.win_label_caption_set
    caption_set.argtypes = [win_p, ctypes.c_char_p]
    caption_set.restype  = ctypes.c_int

    caption_get = so.win_label_caption_get
    caption_get.argtypes = [win_p]
    caption_get.restype  = ctypes.c_char_p

    sidebar_set = so.win_label_sidebar_set
    sidebar_set.argtypes = [win_p, ctypes.c_char_p]
    sidebar_set.restype  = ctypes.c_int

    sidebar_get = so.win_label_sidebar_get
    sidebar_get.argtypes = [win_p]
    sidebar_get.restype  = ctypes.c_char_p

    on_sidebar_set_pre  = ctypes.cast(so.win_label_on_sidebar_set_pre,  hook_p)
    on_sidebar_set_post = ctypes.cast(so.win_label_on_sidebar_set_post, hook_p)

    on_caption_set_pre  = ctypes.cast(so.win_label_on_caption_set_pre,  hook_p)
    on_caption_set_post = ctypes.cast(so.win_label_on_caption_set_post, hook_p)

class iter:
    first = so.win_iter_first
    first.argtypes = [win_p]
    first.restype  = win_p

    last = so.win_iter_last
    last.argtypes = [win_p]
    last.restype  = win_p

    next = so.win_iter_next
    next.argtypes = [win_p]
    next.restype  = win_p

    prev = so.win_iter_prev
    prev.argtypes = [win_p]
    prev.restype  = win_p

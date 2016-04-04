import ctypes
from shared import lib as so

class win_s(ctypes.Structure):
    pass

win_p = ctypes.POINTER(win_s)

class dir:
    up    = 0
    down  = 1
    left  = 2
    right = 3

initsys = so.win_initsys
initsys.argtypes = []
initsys.restype  = ctypes.c_int

killsys = so.win_killsys
killsys.argtypes = []
killsys.restype  = ctypes.c_int

split = so.win_split
split.argtypes = [win_p, ctypes.c_int]
split.restype  = ctypes.c_int

delete = so.win_delete
delete.argtypes = [win_p]
delete.restype  = ctypes.c_int

get_parent = so.win_get_parent
get_parent.argtypes = [win_p]
get_parent.restype  = win_p

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

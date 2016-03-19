import ctypes
from shared import lib as so

from c.hook import hook_p


initsys = so.ui_initsys
initsys.argtypes = []
initsys.restype  = ctypes.c_int

killsys = so.ui_killsys
killsys.argtypes = []
killsys.restype  = ctypes.c_int

refresh= so.ui_draw
refresh.argtypes = []
refresh.restype  = ctypes.c_int

draw = so.ui_draw
draw.argtypes = []
draw.restype  = ctypes.c_int

resize = so.ui_resize
resize.argtypes = []
resize.restype  = ctypes.c_int

class sbar:
    initsys = so.ui_sbar_initsys
    initsys.argtypes = []
    initsys.restype  = ctypes.c_int

    killsys = so.ui_sbar_killsys
    killsys.argtypes = []
    killsys.restype  = ctypes.c_int

    draw = so.ui_sbar_draw
    draw.argtypes = []
    draw.restype  = ctypes.c_int

    set = so.ui_sbar_set
    set.argtypes = [ctypes.c_char_p]
    set.restype  = ctypes.c_int


on_resize  = ctypes.cast(so.ui_on_resize,  ctypes.POINTER(hook_p)).contents

def set_more_left(char):
    so.ui_more_left_char         = ctypes.c_char_p(ctypes.c_char(char))

def set_more_right(char):
    so.ui_more_right_char        = ctypes.c_char_p(ctypes.c_char(char))

def set_window_vertical(char):
    so.ui_window_vertical_char   = ctypes.c_char_p(ctypes.c_char(char))

def set_window_horizontal(char):
    so.ui_window_horizontal_char = ctypes.c_char_p(ctypes.c_char(char))


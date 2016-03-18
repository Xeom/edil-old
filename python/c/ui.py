import ctypes
from shared import lib as so

from c.hook import hook_p

initsys = so.ui_initsys
initsys.argtypes = []
initsys.restype  = ctypes.c_int

display_wintrees = so.ui_display_wintrees
display_wintrees.argtypes = []
display_wintrees.restype  = ctypes.c_int

resize = so.ui_resize
resize.argtypes = []
resize.restype  = ctypes.c_int

killsys = so.ui_killsys
killsys.argtypes = []
killsys.restype  = ctypes.c_int

set_statusbar = so.ui_set_statusbar
set_statusbar.argtypes = [ctypes.c_char_p]
set_statusbar.restype  = ctypes.c_int

display_statusbar = so.ui_display_statusbar
display_statusbar.argtypes = []
display_statusbar.restype  = ctypes.c_int

key_resize = ctypes.cast(so.ui_key_resize, ctypes.POINTER(ctypes.c_int)).contents.value
on_resize  = ctypes.cast(so.ui_on_resize,  ctypes.POINTER(hook_p)).contents

def set_more_left(char):
    so.ui_more_left_char         = ctypes.c_char_p(ctypes.c_char(char))

def set_more_right(char):
    so.ui_more_right_char        = ctypes.c_char_p(ctypes.c_char(char))

def set_window_vertical(char):
    so.ui_window_vertical_char   = ctypes.c_char_p(ctypes.c_char(char))

def set_window_horizontal(char):
    so.ui_window_horizontal_char = ctypes.c_char_p(ctypes.c_char(char))


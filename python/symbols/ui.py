import ctypes
from shared import lib as so

from symbols.hook     import hook_p
from symbols.callback import callback_p
from symbols.win      import win_p

#from symbols.face     import face_p

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

class win:
    initsys = so.ui_win_initsys
    initsys.argtypes = []
    initsys.restype  = ctypes.c_int

    killsys = so.ui_win_killsys
    killsys.argtypes = []
    killsys.restype  = ctypes.c_int

    draw = so.ui_win_draw
    draw.argtypes = []
    draw.restype  = ctypes.c_int

    draw_subs = so.ui_win_draw_subs
    draw_subs.argtypes = [win_p]
    draw_subs.restype  = ctypes.c_int

    class content:
        on_draw_pre  = ctypes.cast(so.ui_win_content_on_draw_pre,  hook_p)
        on_draw_post = ctypes.cast(so.ui_win_content_on_draw_post, hook_p)

        on_draw_line_pre  = ctypes.cast(so.ui_win_content_on_draw_line_pre,  hook_p)
        on_draw_line_post = ctypes.cast(so.ui_win_content_on_draw_line_post, hook_p)

        draw = so.ui_win_content_draw
        draw.argtypes = [win_p]
        draw.restype  = ctypes.c_int

        draw_subs = so.ui_win_content_draw_subs
        draw_subs.argtypes = [win_p]
        draw_subs.restype  = ctypes.c_int

        draw_lines_after = so.ui_win_content_draw_lines_after
        draw_lines_after.argtypes = [win_p, ctypes.c_size_t]
        draw_lines_after.restype  = ctypes.c_int

        draw_line = so.ui_win_content_draw_line
        draw_line.argtypes = [win_p, ctypes.c_size_t]
        draw_line.restype  = ctypes.c_int

    class frame:
        on_draw_pre  = ctypes.cast(so.ui_win_frame_on_draw_pre,  hook_p)
        on_draw_post = ctypes.cast(so.ui_win_frame_on_draw_post, hook_p)

        faceify = so.ui_win_frame_faceify
#        faceify.argtypes = [win_p, face_p]
        faceify.restype  = ctypes.c_int

        draw = so.ui_win_frame_draw
        draw.argtypes = [win_p]
        draw.restype  = ctypes.c_int

        draw_subs = so.ui_win_frame_draw_subs
        draw_subs.argtypes = [win_p]
        draw_subs.restype  = ctypes.c_int

on_resize_pre  = ctypes.cast(so.ui_on_resize_pre,  hook_p)
on_resize_post = ctypes.cast(so.ui_on_resize_post, hook_p)

def set_more_left(char):
    so.ui_more_left_char         = ctypes.c_char_p(ctypes.c_char(char))

def set_more_right(char):
    so.ui_more_right_char        = ctypes.c_char_p(ctypes.c_char(char))

def set_window_vertical(char):
    so.ui_window_vertical_char   = ctypes.c_char_p(ctypes.c_char(char))

def set_window_horizontal(char):
    so.ui_window_horizontal_char = ctypes.c_char_p(ctypes.c_char(char))


import ctypes
from shared import lib as so

import cutil

from symbols.hook     import hook_p
from symbols.callback import callback_p
from symbols.buffer   import buffer_p
from symbols.win      import win_p

class face_s(ctypes.Structure):
    _fields_ = [("bg", ctypes.c_int),
                ("fg", ctypes.c_int),
                ("bright", ctypes.c_char),
                ("under",  ctypes.c_char)]

face_p = ctypes.POINTER(face_s)

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

class updates:
    initsys = so.ui_updates_initsys
    initsys.argtypes = []
    initsys.restype  = ctypes.c_int

    hold = so.ui_updates_hold
    hold.argtypes = [buffer_p]
    hold.restype  = ctypes.c_int

    release = so.ui_updates_release
    release.argtypes = []
    release.restype  = ctypes.c_int

class face:
    colour_black   = cutil.functptr2type(so.face_colour_black,   ctypes.c_int)
    colour_blue    = cutil.functptr2type(so.face_colour_blue,    ctypes.c_int)
    colour_green   = cutil.functptr2type(so.face_colour_green,   ctypes.c_int)
    colour_cyan    = cutil.functptr2type(so.face_colour_cyan,    ctypes.c_int)
    colour_red     = cutil.functptr2type(so.face_colour_red,     ctypes.c_int)
    colour_magenta = cutil.functptr2type(so.face_colour_magenta, ctypes.c_int)
    colour_yellow  = cutil.functptr2type(so.face_colour_yellow,  ctypes.c_int)
    colour_white   = cutil.functptr2type(so.face_colour_white,   ctypes.c_int)

    cursor = ctypes.cast(so.face_cursor, ctypes.c_char_p)

    initsys = so.ui_face_initsys
    initsys.argtypes = []
    initsys.restype  = ctypes.c_int

    killsys = so.ui_face_killsys
    killsys.argtypes = []
    killsys.restype  = ctypes.c_int

    init = so.ui_face_init
    init.argtypes = []
    init.restype  = face_s

    draw = so.ui_face_draw
    draw.argtypes = [face_s, ctypes.c_uint]
    draw.restype  = ctypes.c_int

    draw_at = so.ui_face_draw_at
    draw_at.argtypes = [face_s,
                        ctypes.c_int,
                        ctypes.c_int,
                        ctypes.c_uint,
                        ctypes.c_uint]

    serialize = so.ui_face_serialize
    serialize.argtypes = [face_s, ctypes.c_short]
    serialize.restype  = ctypes.POINTER(ctypes.c_char)

    deserialize_face = so.ui_face_deserialize_face
    deserialize_face.argtypes = [ctypes.c_char_p]
    deserialize_face.restype  = face_s

    deserialize_length = so.ui_face_deserialize_length
    deserialize_length.argtypes = [ctypes.c_char_p]
    deserialize_length.restype  = ctypes.c_short

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

    corner_char = ctypes.cast(so.ui_win_corner_char,
                              ctypes.POINTER(ctypes.c_char))
    horizontal_char = ctypes.cast(so.ui_win_horizontal_char,
                                  ctypes.POINTER(ctypes.c_char))
    vertical_char = ctypes.cast(so.ui_win_vertical_char,
                                ctypes.POINTER(ctypes.c_char))

    frame_face = ctypes.cast(so.ui_win_corner_char, face_p)
    frame_sel_face = ctypes.cast(so.ui_win_frame_sel_face, face_p)


    class content:
        on_draw_pre  = ctypes.cast(so.ui_win_content_on_draw_pre,  hook_p)
        on_draw_post = ctypes.cast(so.ui_win_content_on_draw_post, hook_p)
        on_draw_line_pre  = ctypes.cast(so.ui_win_content_on_draw_line_pre,
                                        hook_p)
        on_draw_line_post = ctypes.cast(so.ui_win_content_on_draw_line_post,
                                        hook_p)

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

        get_cursor_offset = so.ui_win_content_get_cursor_offset
        get_cursor_offset.argtypes = [win_p, ctypes.c_ulong]
        get_cursor_offset.restype  = ctypes.c_size_t

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


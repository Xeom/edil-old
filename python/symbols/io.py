import ctypes
from shared import lib as so

from symbols.hook   import hook_p
from symbols.vec    import vec_p
from symbols.buffer import buffer_p


class key_s(ctypes.Structure):
    _fields_ = [("modifiers", ctypes.c_char),
                ("keyname",   ctypes.c_char *
                 ctypes.cast(so.io_key_name_len, ctypes.POINTER(ctypes.c_int)).contents.value)]

class key:
    set_name = so.io_key_set_name
    set_name.argtypes = [key_s, ctypes.c_char_p]
    set_name.restype  = key_s

    initsys = so.io_key_initsys
    initsys.argtypes = []
    initsys.restype  = ctypes.c_int

    killsys = so.io_key_killsys
    killsys.argtypes = []
    killsys.restype  = ctypes.c_int

    str = so.io_key_str
    str.argtypes = [key_s]
    str.restype  = ctypes.POINTER(ctypes.c_char)

    handle_chr = so.io_key_handle_chr
    handle_chr.argtypes = [ctypes.c_int]
    handle_chr.restype  = ctypes.c_int

    resize   = ctypes.cast(so.io_key_resize,   ctypes.POINTER(ctypes.c_int)).contents.value
    name_len = ctypes.cast(so.io_key_name_len, ctypes.POINTER(ctypes.c_int)).contents.value

    on_key = ctypes.cast(so.io_key_on_key, hook_p)

class file:
    dump_vec = so.file_dump_vec
    dump_vec.argtypes = [vec_p, ctypes.c_void_p]
    dump_vec.restype  = ctypes.c_int

    dump_buffer = so.file_dump_buffer
    dump_buffer.argtypes = [buffer_p, ctypes.c_void_p]
    dump_buffer.restype  = ctypes.c_int

    read_buffer = so.file_read_buffer
    read_buffer.argtypes = [buffer_p, ctypes.c_void_p]
    read_buffer.restype  = ctypes.c_int

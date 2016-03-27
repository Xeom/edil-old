import ctypes
from shared import lib as so

from c.hook import hook_p

keyname_len = ctypes.cast(so.io_keyname_len, ctypes.POINTER(ctypes.c_int)).contents.value

class key_s(ctypes.Structure):
    _fields_ = [("modifiers", ctypes.c_char),
                ("keyname",   ctypes.c_char * keyname_len)]

initsys = so.io_initsys
initsys.argtypes = []
initsys.restype  = ctypes.c_int

killsys = so.io_killsys
killsys.argtypes = []
killsys.restype  = ctypes.c_int

key_str = so.io_key_str
key_str.argtypes = [key_s]
key_str.restype  = ctypes.POINTER(ctypes.c_char)

handle_chr = so.io_handle_chr
handle_chr.argtypes = [ctypes.c_int]
handle_chr.restype  = ctypes.c_int

on_keypress  = ctypes.cast(so.io_on_keypress, hook_p)

key_resize = ctypes.cast(so.io_key_resize, ctypes.POINTER(ctypes.c_int)).contents.value

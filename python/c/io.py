import ctypes
from shared import lib as so

from c.hook import hook_p

class key_s(ctypes.Structure):
    _fields_ = [("modifiers", ctypes.c_char),
                ("keyname",   ctypes.c_char * 4)]

initsys = so.io_initsys
initsys.argtypes = []
initsys.restype  = ctypes.c_int

killsys = so.io_killsys
killsys.argtypes = []
killsys.restype  = ctypes.c_int

key_str = so.io_key_str
key_str.argtypes = [key_s]
key_str.restype  = ctypes.c_char_p

handle_chr = so.io_handle_chr
handle_chr.argtypes = [ctypes.c_int]
handle_chr.restype  = ctypes.c_int

on_keypress  = ctypes.cast(so.io_on_keypress,  ctypes.POINTER(hook_p)).contents

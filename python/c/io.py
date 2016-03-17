import ctypes
from shared import lib as so

class key_s(ctypes.Structure):
    pass

key_str = so.io_key_str
key_str.argtypes = [key_s]
key_str.restype  = ctypes.c_char_p

handle_chr = so.io_handle_chr
handle_chr.argtypes = [ctypes.c_int]
handle_chr.restype  = ctypes.c_int

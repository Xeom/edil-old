import os

import ctypes
from shared import lib as so

def functptr2type(fptr, type):
    return ctypes.cast(fptr, ctypes.POINTER(type)).contents

def ptr2int(p):
    if isinstance(p, ctypes._Pointer):
        return ctypes.addressof(p.contents)
    else:
        return p.value

def ptreq(a, b):
    return ctypes.cast(a, ctypes.c_void_p).value == ctypes.cast(b, ctypes.c_void_p).value

def isnull(ptr):
    return not bool(ptr)#ctypes.cast(ptr, ctypes.c_void_p).value == None

def str2char(s):
    if isinstance(s, str):
        return ctypes.c_char_p(s.encode("ascii"))

    else:
        return ctypes.c_char_p(s)

so.fopen.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
so.fopen.restype  = ctypes.c_void_p

so.fclose.argtypes = [ctypes.c_void_p]
so.fclose.restype  = ctypes.c_int

def fptr_open(path, mode):
    path = os.path.abspath(path)

    fptr = so.fopen(str2char(path), str2char(mode))

    if isnull(fptr):
        raise Exception("No.")

    return fptr

def fptr_close(fptr):
    so.fclose(fptr)


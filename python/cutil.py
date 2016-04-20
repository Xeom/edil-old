import os

import ctypes
from shared import lib as so


def ptreq(a, b):
    return ctypes.cast(a, ctypes.c_void_p).value == ctypes.cast(b, ctypes.c_void_p).value


def isnull(ptr):
    return ctypes.cast(ptr, ctypes.c_void_p).value == None

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

    if not os.path.isfile(path):
        raise IOError("File not found '{}'".format(path))

    fptr = so.fopen(str2char(path), str2char(mode))

    if isnull(fptr):
        raise Exception("No.")

    return fptr

def fptr_close(fptr):
    so.fclose(fptr)


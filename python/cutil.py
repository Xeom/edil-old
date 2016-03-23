import ctypes

def isnull(ptr):
    return ctypes.cast(ptr, ctypes.c_void_p).value == None


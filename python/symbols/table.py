import ctypes
from shared import lib as so

class table_s(ctypes.Structure):
    pass

table_p = ctypes.POINTER(table_s)

len = so.table_len
len.argtypes = [table_p]
len.restype  = ctypes.c_size_t

set = so.table_set
set.argtypes = [table_p, ctypes.c_void_p, ctypes.c_void_p]
set.restype  = ctypes.c_int

get = so.table_get
get.argtypes = [table_p, ctypes.c_void_p]
get.restype  = ctypes.c_void_p

delete = so.table_delete
delete.argtypes = [table_p, ctypes.c_void_p]
delete.restype  = ctypes.c_int

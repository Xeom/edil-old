import ctypes
from shared import lib as so

from c.hook import hook_p
from c.vec  import vec_p

class flag:
    readonly = 1
    modified = 2

class buffer_s(ctypes.Structure):
    pass

buffer_p = ctypes.POINTER(buffer_s)

lineno = ctypes.c_size_t

on_create = ctypes.cast(so.buffer_on_create, hook_p)
on_delete = ctypes.cast(so.buffer_on_delete, hook_p)

init = so.buffer_init
init.argtypes = []
init.restype  = buffer_p

free = so.buffer_free
free.argtypes = [buffer_p]
free.restype  = None

insert = so.buffer_insert
insert.argtypes = [buffer_p, lineno]
insert.restype  = ctypes.c_int

delete = so.buffer_delete
delete.argtypes = [buffer_p, lineno]
delete.restype  = ctypes.c_int

get_line = so.buffer_get_line
get_line.argtypes = [buffer_p, lineno]
get_line.restype  = vec_p

set_line = so.buffer_set_line
set_line.argtypes = [buffer_p, lineno, vec_p]
set_line.restype  = ctypes.c_int

get_flag = so.buffer_get_flag
get_flag.argtypes = [buffer_p, ctypes.c_uint]
get_flag.restype  = ctypes.c_int

enable_flag = so.buffer_enable_flag
enable_flag.argtypes = [buffer_p, ctypes.c_uint]
enable_flag.restype  = ctypes.c_int

disable_flag = so.buffer_disable_flag
disable_flag.argtypes = [buffer_p, ctypes.c_uint]
disable_flag.restype  = ctypes.c_int

len = so.buffer_len
len.argtypes = [buffer_p]
len.restype  = lineno

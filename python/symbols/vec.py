import ctypes
from shared import lib as so

import cutil

class vec_s(ctypes.Structure):
    _fields_ = [ ("data",     ctypes.c_void_p),
                 ("width",    ctypes.c_size_t),
                 ("length",   ctypes.c_size_t),
                 ("capacity", ctypes.c_size_t) ]

vec_p = ctypes.POINTER(vec_s)

class VecErr(Exception):
    pass

def check_nonzro_return(value, func, args):
    if value == -1:
        raise VecErr()

    return value

def check_null_return(value, func, args):
    if cutil.isnull(value):
        raise VecErr()

    return value

def check_invalid_index(value, func, args):
    sizemax = (1 << ctypes.sizeof(ctypes.c_size_t) * 8) - 1

    if value == sizemax:
        raise VecErr(err_str())

    return value

# vec *vec_init(size_t width)
init = so.vec_init
init.argtypes = [ctypes.c_size_t]
init.restype  = vec_p
init.errcheck = check_null_return

# void vec_free(vec *v)
free = so.vec_free
free.argtypes = [vec_p]
free.restype  = None

item = so.vec_item
item.argtypes = [vec_p, ctypes.c_size_t]
item.restype  = ctypes.c_void_p
item.errcheck = check_null_return

delete = so.vec_delete
delete.argtypes = [vec_p, ctypes.c_size_t, ctypes.c_size_t]
delete.restype  = ctypes.c_int
delete.errcheck = check_nonzro_return

insert = so.vec_insert
insert.argtypes = [vec_p, ctypes.c_size_t, ctypes.c_size_t, ctypes.c_void_p]
insert.restype  = ctypes.c_int
insert.errcheck = check_nonzro_return

len = so.vec_len
len.argtypes = [vec_p]
len.restype  = ctypes.c_size_t

# size_t vec_find(vec *v, const void *value)
find = so.vec_find
find.argtypes = [vec_p, ctypes.c_void_p]
find.restype  = ctypes.c_size_t
find.errcheck = check_invalid_index

# size_t vec_rfind(vec *v, const void *value)
rfind = so.vec_rfind
rfind.argtypes = [vec_p, ctypes.c_void_p]
rfind.restype  = ctypes.c_size_t
rfind.errcheck = check_invalid_index

#vec *vec_cut(vec *v, size_t index, size_t n)
cut = so.vec_cut
cut.argtypes = [vec_p, ctypes.c_size_t, ctypes.c_size_t]
cut.restype  = vec_p
cut.errcheck = check_null_return

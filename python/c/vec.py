import ctypes
from shared import lib as so

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
        raise VecErr(err_str())

    return value

def check_null_return(value, func, args):
    cast = ctypes.cast(value, ctypes.c_void_p)

    if cast.value == None:
        raise VecErr(err_str())

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

insert = so.vec_delete
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

# const char *vec_err_str(void)
err_str = so.vec_err_str
err_str.argtypes = []
err_str.restype  = ctypes.c_char_p

class Vec:
    def __init__(self, struct, type):
        self.struct = struct
        self.type = type
        self.type_p = ctypes.POINTER(type)

    def __iter__(self):
        for i in range(self.__len__()):
            import sys
            print(i, file=sys.stderr)
            yield self.get(i)

    def __len__(self):
        return self.len()

    def getptr(self, value):
        if not isinstance(value, self.type):
            value = self.type(value)

        return self.type_p(value)

    def get(self, index):
        ptr = item(self.struct, index)
        ptr = ctypes.cast(ptr, self.type_p)
        return ptr.contents

    def len(self):
        return len(self.struct)

    def set(self, index, value):
        ptr = ctypes.cast(item(self.struct, index), self.type_p)
        ptr.contents = value
        set(self.struct, index, ptr)

    def find(self, value):
        ptr = self.getptr(value)
        return find(self.struct, ptr)

    def rfind(self, value):
        ptr = self.getptr(value)
        return rfind(self.struct, ptr)

    def insert(self, index, value):
        ptr = self.getptr(value)
        insert(self.struct, index, 1, ptr)

    def delete(self, index, n):
        delete(self.struct, index, n)

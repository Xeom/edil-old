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

# int vec_push(vec *v, const void *data)
push = so.vec_push
push.argtypes = [vec_p, ctypes.c_void_p]
push.restype  = ctypes.c_int
push.errcheck = check_nonzro_return

# void *vec_pop(vec *v)
pop = so.vec_pop
pop.argtypes = [vec_p]
pop.restype  = ctypes.c_void_p
pop.errcheck = check_null_return

# void *vec_get(vec *v, size_t index)
get = so.vec_get
get.argtypes = [vec_p, ctypes.c_size_t]
get.restype  = ctypes.c_void_p
get.errcheck = check_null_return

# int vec_set(vec *v, size_t index, const void *data)
set = so.vec_set
set.argtypes = [vec_p, ctypes.c_size_t, ctypes.c_void_p]
set.restype  = ctypes.c_int
set.errcheck = check_nonzro_return

# int vec_trim(vec *v, size_t amount)
trim = so.vec_trim
trim.argtypes = [vec_p, ctypes.c_size_t]
trim.restype  = ctypes.c_int
trim.errcheck = check_nonzro_return

# vec *vec_slice(vec *v, size_t start, size_t end)
slice = so.vec_slice
slice.argtypes = [vec_p, ctypes.c_size_t, ctypes.c_size_t]
slice.restype  = vec_p
slice.errcheck = check_null_return

# int vec_append(vec *v, vec *other)
append = so.vec_append
append.argtypes = [vec_p, vec_p]
append.restype  = ctypes.c_int
append.errcheck = check_nonzro_return

# size_t vec_len(vec *v)
len = so.vec_len
len.argtypes = [vec_p]
len.restype  = ctypes.c_size_t
len.errcheck = check_invalid_index

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

# int vec_insert(vec *v, size_t i, const void *value)
insert = so.vec_insert
insert.argtypes = [vec_p, ctypes.c_size_t, ctypes.c_void_p]
insert.restype  = ctypes.c_int
insert.errcheck = check_nonzro_return

# int vec_remove(vec *v, const void *value)
remove = so.vec_remove
remove.argtypes = [vec_p, ctypes.c_void_p]
remove.restype  = ctypes.c_int
remove.errcheck = check_nonzro_return

# int vec_delete(vec *v, size_t i)
delete = so.vec_delete
delete.argtypes = [vec_p, ctypes.c_size_t]
delete.errcheck = check_nonzro_return

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

    def push(self, value):
        ptr = self.getptr(value)
        push(self.struct, ptr)

    def pop(self):
        ptr = pop(self.struct)
        ptr = ctypes.cast(ptr, self.type_p)
        return ptr.contents

    def get(self, index):
        ptr = get(self.struct, index)
        ptr = ctypes.cast(ptr, self.type_p)
        return ptr.contents

    def len(self):
        return len(self.struct)

    def set(self, index, value):
        ptr = self.getptr(value)
        set(self.struct, index, ptr)

    def trim(self, size):
        trim(self.struct, size)

    def slice(self, start, end):
        ptr = slice(self.struct, start, end)
        newvec = Vec(self.type)
        newvec.struct = ptr
        return newvec

    def append(self, other):
        append(self.struct, other.struct)

    def find(self, value):
        ptr = self.getptr(value)
        return find(self.struct, ptr)

    def rfind(self, value):
        ptr = self.getptr(value)
        return rfind(self.struct, ptr)

    def insert(self, index, value):
        ptr = self.getptr(value)
        insert(self.struct, index, ptr)

    def remove(self, value):
        ptr = self.getptr(value)
        remove(self.struct, ptr)

    def delete(self, index):
        delete(self.struct, index)

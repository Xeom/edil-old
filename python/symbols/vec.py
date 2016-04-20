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

# const char *vec_err_str(void)
err_str = so.vec_err_str
err_str.argtypes = []
err_str.restype  = ctypes.c_char_p

class Vec:
    def __init__(self, struct, type):
        self.struct = struct
        self.type = type
        self.type_p = ctypes.POINTER(type)

    @classmethod
    def Type(cls, type):
        class Rtn(cls):
            def __init__(self, struct):
                cls.__init__(self, struct, type)

        return Rtn

    def getptr(self, value):
        if not isinstance(value, self.type):
            value = self.type(value)

        return self.type_p(value)

    def __iter__(self):
        for i in range(self.__len__()):
            yield self[i]

    def __len__(self):
        return len(self.struct)

    def __getitem__(self, index):
        import sys
        if isinstance(index, slice):
            start = index.start
            stop  = index.stop

            if index.step != None:
                raise NotImplemented

            if start == None:
                start = 0

            if stop == None:
                stop = -1

            return self.getslice(start, stop)

        ptr = item(self.struct, index)
        ptr = ctypes.cast(ptr, self.type_p)
        return ptr.contents

    def getslice(self, start, end):
        rtn    = VecFreeOnDel(init(ctypes.sizeof(self.type)), self.type)
        length = self.__len__()

        if start >= length:
            return rtn

        if end < 0:
            end = length - end

        amount = min(end, length - 1)

        insert(rtn.struct, 0, amount, item(self.struct, start))

        return rtn

    def __setitem__(self, index, value):
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

    def __iadd__(self, other):
        if not other.__len__():
            return self

        insert(self.struct, self.__len__(), other.__len__(), item(other.struct, 0))

        return self

    def delete(self, index, n):
        delete(self.struct, index, n)

class VecFreeOnDel(Vec):
    def __del__(self):
        free(self.struct)

def str2vec(s):
    if isinstance(s, str):
        s = s.encode("ascii")

    struct = init(ctypes.sizeof(ctypes.c_char))
    rtn = VecFreeOnDel(struct, ctypes.c_char)

    insert(struct, 0, bytes.__len__(s), s)

    return rtn

def vec2str(v):
    return b"".join(map(lambda c:symbols.value, v))

import ctypes

import symbols.vec

from core.container import Container, StructObject

class Vec(StructObject):
    PtrType = symbols.vec.vec_p

    def __init__(self, struct, typ):
        StructObject.__init__(self, struct)
        self.typ_s = typ
        self.typ_p = ctypes.POINTER(typ)
        self.width = ctypes.sizeof(typ)

    @classmethod
    def Type(cls, typ):
        class Rtn(cls):
            def __init__(self, struct):
                cls.__init__(self, struct, typ)

    @classmethod
    def new(cls, typ):
        ptr = symbols.vec.init(ctypes.sizeof(typ))
        return cls(ptr, typ)

    @classmethod
    def from_string(cls, string):
        if isinstance(string, tr):
            s = s.encode("ascii")

        rtn = cls.new(ctypes.c_char)
        rtn.insert_bytes(0, string)

        return rtn

    def pyval_ptr(self, val):
        if isinstance(val, self.typ_p):
            return val

        elif not isinstance(val, self.typ_s):
            val = self.typ_s(val)

        return self.typ_p(val)

    def delete(self, index, n):
        symbols.vec.delete(self.struct, index, n)

    def find(self, value):
        ptr = self.pyval_ptr(value)
        return symbols.vec.find(self.struct, ptr)

    def rfind(self, value):
        ptr = self.pyval_ptr(value)
        return symbols.vec.rfind(self.struct, ptr)

    def insert(self, index, value):
        ptr = self.pyval_ptr(value)
        symbols.vec.insert(self.struct, index, 1, ptr)

    def insert_bytes(elf, index, data):
        num = self.width * len(self)
        symbols.vec.insert(self.struct, index, num, data)

    def getslice(self, start=0, end=-1):
        rtn    = VecFreeOnDel.new(self.typ_s)
        length = len(self)

        if start >= length:
            return rtn

        end   %= length
        amount = min(end, length - 1)

        symbols.vec.insert(rtn.struct, 0, amount,
                           symbols.vec.item(self.struct, start))

        return rtn

    def free(self):
        symbols.vec.free(self.struct)

    def __len__(self):
        if not self.struct:
            raise Exception()
        return symbols.vec.len(self.struct)

    def __bytes__(self):
        num = self.width * len(self)

        if num:
            ptr = symbols.vec.item(self.struct, 0)
            rtn = ctypes.cast(ptr, ctypes.POINTER(num * ctypes.c_char))

            return rtn.contents.value

        else:
            return b""

    def __iadd__(self, other):
        if len(other):
            symbols.vec.insert(self.struct, len(self), len(other),
                               symbols.vec.item(other.struct, 0))

        return self


    def __setitem__(self, index, value):
        ptr = ctypes.cast(item(self.struct, index), self.typ_p)
        ptr.contents = value

    def __iter__(self):
        for index in range(len(self)):
            ptr = symbols.vec.item(self.struct, index)
            yield self.typ_s.from_address(ptr)

    def __delitem__(self, index):
        self.delete(index, 1)

    def __getitem__(self, index):
        if isinstance(index, slice):
            start = index.start
            stop  = index.stop

            if index.step != None:
                raise NotImplemented

            start = start if start != None else 0
            end   = end   if end   != None else -1

            return self.getslice(start, end)

        ptr  = symbols.vec.item(self.struct, index)
        return self.typ_s.from_address(ptr)

class VecFreeOnDel(Vec):
    def __del__(self):
        self.free()

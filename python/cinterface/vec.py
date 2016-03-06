import c.vec
import ctypes
from cinterface.structproto import Struct

class Vec(Struct):
    cfuncts = c.vec

    def __init__(self, type):
        Struct.__init__(self)
        self.type = type
        self.type_p = ctypes.POINTER(type)

    def __iter__(self):
        return (self.get(i) for i in range(len(self)))

    def __len__(self):
        return self.len()

    def init(self):
        self.struct = c.vec.init(ctypes.sizeof(self.type))

    def getptr(self, value):
        if not isinstance(value, self.type):
            value = self.type(value)

        return self.type_p(value)

    def push(self, value):
        ptr = self.getptr(value)
        c.vec.push(self.struct, ptr)

    def pop(self):
        ptr = c.vec.pop(self.struct)
        ptr = ctypes.cast(ptr, self.type_p)
        return ptr.contents

    def get(self, index):
        ptr = c.vec.get(self.struct, index)
        ptr = ctypes.cast(ptr, self.type_p)
        return ptr.contents

    def set(self, index, value):
        ptr = self.getptr(value)
        c.vec.set(self.struct, index, ptr)

    def trim(self, size):
        c.vec.trim(self.struct, size)

    def slice(self, start, end):
        ptr = c.vec.slice(self.struct, start, end)
        newvec = Vec(self.type)
        newvec.struct = ptr
        return newvec

    def append(self, other):
        c.vec.append(self.struct, other.struct)

    def len(self):
        return c.vec.len(self.struct)

    def find(self, value):
        ptr = self.getptr(value)
        return c.vec.find(self.struct, ptr)

    def rfind(self, value):
        ptr = self.getptr(value)
        return c.vec.rfind(self.struct, ptr)

    def insert(self, index, value):
        ptr = self.getptr(value)
        c.vec.insert(self.struct, index, ptr)

    def remove(self, value):
        ptr = self.getptr(value)
        c.vec.remove(self.struct, ptr)

    def delete(self, index):
        c.vec.delete(self.struct, index)

import symbols.table
import cutil
import ctypes

class Table:
    def __init__(self, ptr):
        self.struct = ptr

    def __getitem__(self, key):
        kptr = self.valuetoptr(key)
        vptr = symbols.table.get(self.struct, kptr)
        return self.ptrtovalue(vptr)

    def __setitem__(self, key, value):
        kptr = self.valuetoptr(key)
        vptr = self.valuetoptr(value)
        symbols.table.set(self.struct, kptr, vptr)

class CharTable(Table):
    def valuetoptr(self, val):
        ptr = cutil.str2char(val)
        return ctypes.cast(ptr, ctypes.c_void_p)

    def ptrtovalue(self, ptr):
        ptr = ctypes.cast(ptr, ctypes.c_char_p)
        return ptr.value

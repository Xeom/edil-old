import symbols.table
import cutil
import ctypes

class Table:
    def __init__(self, ptr):
        self.struct = ptr

    def __getitem__(self, key):
        kptr = self.keytoptr(key)
        vptr = symbols.table.get(self.struct, kptr)

        if cutil.isnull(vptr):
            return None

        return self.ptrtovalue(vptr)

    def __setitem__(self, key, value):
        kptr = self.keytoptr(key)
        vptr = self.valuetoptr(value)
        symbols.table.set(self.struct, kptr, vptr)

class CTable(Table):
    def __getitem__(self, key):
        if isinstance(key, str):
            key = key.encode("ascii")

        return symbols.table.cget(self.struct, key)

    def __setitem__(self, key, value):
        if isinstance(key, str):
            key = key.encode("ascii")

        if isinstance(value, str):
            value = value.encode("ascii")

        symbols.table.cset(self.struct, key, value)

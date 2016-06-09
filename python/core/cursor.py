import weakref

import cutil
import ctypes

import symbols.cursor

class CursorContainer:
    def __init__(self):
        self.by_ptr = weakref.WeakValueDictionary()

    def mount(self):pass
#        @hooks.delete_struct(50)
#        def handle_delete(struct):
#            ptr = cutil.ptr2int(struct)
#            w   = self.by_ptr.get(ptr)
#
#            if w == None:
#                return
#
#            w.valid = False
#            del self.by_ptr[ptr]
#
#        self.handle_delete = handle_delete

    def __call__(self, struct):
        ptr = cutil.ptr2int(struct)
        w   = self.by_ptr.get(ptr)

        if w != None:
            return w

        w = CursorObj(struct)
        self.by_ptr[ptr] = w

        return w

class CursorObj:
    def __init__(self, ptr):
        self.struct = ctypes.cast(ptr, symbols.cursor.cursor_p)

    @property
    def struct(self):
        if not self.valid:
            raise Exception("Invalid cursor")

        return self._struct

    @struct.setter
    def struct(self, value):
        self.valid   = True
        self._struct = value

    @property
    def ln(self):
        return symbols.cursor.get_ln(self.struct)

    @ln.setter
    def ln(self, v):
        symbols.cursor.set_ln(self.struct, v)

    @property
    def cn(self):
        return symbols.cursor.get_cn(self.struct)

    @cn.setter
    def cn(self, v):
        symbols.cursor.set_cn(self.struct, v)

    @property
    def buffer(self):
        return symbols.cursor.get_buffer(self.struct)

    def move_cols(self, n):
        symbols.cursor.move_cols(self.struct, n)

    def move_lines(self, n):
        symbols.cursor.move_lines(self.struct, n)

    def insert(self, string):
        symbols.cursor.insert(self.struct, string)

    def delete(self, n):
        symbols.cursor.delete(self.struct, n)

    def enter(self):
        symbols.cursor.enter(self.struct)

    def open_stream(self):
        fptr = symbols.buffer.log.point_stream(self.struct)

        return cutil.fptr2file(fptr, "wb", 1)


Cursor = CursorContainer()

def initsys():
    symbols.cursor.initsys()
    symbols.cursor.point.initsys()

def get_buffer_selected(buf):
    return Cursor(symbols.cursor.buffer_selected(buf.struct))

def get_selected():
    return Cursor(symbols.cursor.selected())

def spawn(buf, typ):
    return Cursor(symbols.cursor.spawn(buf.struct, typ))

class types:
    point = symbols.cursor.point.type

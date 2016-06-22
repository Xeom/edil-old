import weakref

import cutil
import ctypes

import symbols.cursor

from core.buffer import Buffer

class CursorType:
    def __init__(self, instclass):
        self.instclass = instclass

        fields  =  symbols.cursor.cursor_type_s._fields_

        fnames  = [name for (name, typ) in fields]
        ftypes  = [typ  for (name, typ) in fields]

        functs  = [getattr(self, "_" + name) if hasattr(instclass, name) else 0
                   for name in fnames]

        functs[0] = self._init
        print(list(zip(ftypes, functs)))
        cfuncts = [typ(f) for (f, typ) in zip(functs, ftypes)]

        self.struct    = ctypes.pointer(symbols.cursor.cursor_type_s(*cfuncts))
        self.instances = {}

    def _free(self, ptr):
        del self.instances[ptr]

    def _init(self, bufptr):
        new = self.instclass()

        self.instances[id(new)] = new

        if hasattr(new, "init"):
            rtn = new.init(Buffer(bufptr))

            if isinstance(rtn, int):
                return rtn

        return id(new)

    def call(self, ptr, methname, *args):
        inst = self.instances[ptr]

        return getattr(inst, methname)(*args)

    def _free(self, ptr):
        del self.instances[ptr]

    def _get_ln(self, ptr):
        return self.call(ptr, "get_ln")

    def _get_cn(self, ptr):
        return self.call(ptr, "get_cn")

    def _set_ln(self, ptr, ln):
        self.call(ptr, "set_ln", ln)

    def _set_cn(self, ptr, cn):
        self.call(ptr, "set_cn", cn)

    def _move_lines(self, ptr, n):
        self.call(ptr, "move_lines", n)

    def _move_cols(self, ptr):
        self.call(ptr, "move_cols", n)

    def _insert(self, ptr, string):
        self.call(ptr, "insert", string)

    def _delete(self, ptr, n):
        self.call(ptr, "delete", n)

    def _enter(self, ptr):
        self.call(ptr, "enter")

    def _activate(self, ptr):
        self.call(ptr, "activate")

    def _deactivate(self, ptr):
        self.call(ptr, "deactivate")

    def find_instance(self, cursor):
        return self.instances[symbols.cursor.get_ptr(cursor.struct)]

class CursorTypeFromPtr:
    def __init__(self, ptr):
        self.struct = ptr

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

    def activate(self):
        symbols.cursor.activate(self.struct)

    def deactivate(self):
        symbols.cursor.deactivate(self.struct)

    def open_stream(self):
        fptr = symbols.buffer.log.point_stream(self.struct)

        return cutil.fptr2file(fptr, "wb", 1)

Cursor = CursorContainer()

def initsys():
    symbols.cursor.initsys()
    symbols.cursor.point.initsys()
    symbols.cursor.region.initsys()

def get_buffer_selected(buf):
    return Cursor(symbols.cursor.buffer_selected(buf.struct))

def get_selected():
    return Cursor(symbols.cursor.selected())

def spawn(buf, typ):
    return Cursor(symbols.cursor.spawn(buf.struct, typ.struct))

def select_next(buf):
    symbols.cursor.select_next(buf.struct)

def select_last(buf):
    symbols.cursor.select_last(buf.struct)

def delete_selected(buf):
    symbols.cursor.free(symbols.cursor.buffer_selected(buf.struct))

class types:
    point  = CursorTypeFromPtr(symbols.cursor.point.type)
    region = CursorTypeFromPtr(symbols.cursor.region.type)

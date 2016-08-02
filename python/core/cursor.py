import weakref

import cutil
import ctypes

import symbols.cursor

from core.buffer import Buffer

class CursorType:
    """A class representing a type of cursor.

    This class provides a way to turn a python class into a cursor_type that can
    be used by the C cursor system.

    A CursorType is provided an instance class on initialization, an instance of
    which will be used to represent each instance of the cursor. This instance
    class may have methods optionally defined for any of the methods in the
    cursor_type struct. These should be called the same name as they are in the
    struct. e.g.
        def move_cols(self, n):
            self.pos += n

        def get_cn(self):
            return self.pos
    """

    def __init__(self, instclass):
        """Initialize self.

        Arguments:
            instclass (class): A class, an instance of which will be used to
                               represent each instance of this type of cursor.
        """

        self.instclass = instclass

        # Get a list of the types and names of items in the cursor_type struct.
        fields  =  symbols.cursor.cursor_type_s._fields_

        # Split these into two lists of names and types
        fnames  = [name for (name, typ) in fields]
        ftypes  = [typ  for (name, typ) in fields]

        # Create a list of _* functions of this class instance if they exist in
        # the instclass. Otherwise simply have 0, since this will make a NULL
        # ptr.
        functs  = [getattr(self, "_" + name) if hasattr(instclass, name) else 0
                   for name in fnames]

        # Always add self._init regardless of whether it's part of the instclass
        functs[0] = self._init
        functs[1] = self._free

        # Map all these functions to the appropriate types
        cfuncts = [typ(f) for (f, typ) in zip(functs, ftypes)]

        # Make a cursor_type struct from the callback functions
        self.struct    = ctypes.pointer(symbols.cursor.cursor_type_s(*cfuncts))

        # A list for keeping track of instances of this cursor.
        self.instances = {}

    # A load of methods prefixed with _ are used to wrap instance functions

    def _free(self, ptr):
        inst = self.instances[ptr]
        del self.instances[ptr]

        if hasattr(inst, "free"):
            new.free()

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
        try:
            self.call(ptr, "set_ln", ln)
        except:
            return -1
        return 0

    def _set_cn(self, ptr, cn):
        try:
            self.call(ptr, "set_cn", cn)
        except:
            return -1
        return 0

    def _move_lines(self, ptr, n):
        try:
            self.call(ptr, "move_lines", n)
        except:
            return -1
        return 0

    def _move_cols(self, ptr):
        try:
            self.call(ptr, "move_cols", n)
        except:
            return -1
        return 0

    def _insert(self, ptr, string):
        try:
            self.call(ptr, "insert", string)
        except:
            return -1
        return 0

    def _delete(self, ptr, n):
        try:
            self.call(ptr, "delete", n)
        except:
            return -1
        return 0

    def _enter(self, ptr):
        try:
            self.call(ptr, "enter")
        except:
            return -1
        return 0

    def _activate(self, ptr):
        try:
            self.call(ptr, "activate")
        except:
            return -1
        return 0

    def _deactivate(self, ptr):
        try:
            self.call(ptr, "deactivate")
        except:
            return -1
        return 0

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
        return Buffer(symbols.cursor.get_buffer(self.struct))

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

snap_blacklist = symbols.vec.Vec(symbols.cursor.snap_blacklist,
                                 symbols.cursor.cursor_type_p)

class types:
    point  = CursorTypeFromPtr(symbols.cursor.point.type)
    region = CursorTypeFromPtr(symbols.cursor.region.type)

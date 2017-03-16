import weakref

import cutil
import ctypes

import symbols.cursor

from core.hook      import Hook
from core.buffer    import Buffer
from core.vec       import Vec
from core.container import StructObject, Container

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

        # A list for keeping track of instances of this cursor.
        self.instances = {}

        self.struct = ctypes.pointer(symbols.cursor.cursor_type_s(*cfuncts))

    # The following functions, prefixed with _, are turned into C function
    # pointer
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

        return 0

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

    def is_instance(self, cursor):
        return symbols.cursor.get_ptr(cursor.struct) in self.instances

class CursorTypeFromPtr:
    """Class used to represent a cursor type defined outside python.

    This is a really simple class, since for the time being, it only represents
    C cursor types which are static attributes of the shared object. We only
    need to contain a .struct attribute.
    """

    def __init__(self, struct):
        """Initialize an instance of this class.

        Arguments:
            struct (cursor_type_p): A pointer to the cursor type to be
                represented.
        """
        self.struct = struct

class CursorObj(StructObject):
    """A class used to represent an instance of a cursor.

    This class wraps a pointer to a cursor structure, and allows us to call
    python functions on it. Even cursors whose methods are implemented in python
    are managed and called through this object - Their python methods are
    converted to C function pointers and called from their structure.

    The description of functions in this class is based on what they are
    intended to do. Any particular cursor type may interpret them however it
    wants.
    """
    PtrType = symbols.cursor.cursor_p

    @property
    def ln(self):
        """Return the current linenumber of this cursor."""
        return symbols.cursor.get_ln(self.struct)

    @ln.setter
    def ln(self, v):
        """Set the current linenumber of this cursor."""
        symbols.cursor.set_ln(self.struct, v)

    @property
    def cn(self):
        """Return the current columnnumber of this cursor."""
        return symbols.cursor.get_cn(self.struct)

    @cn.setter
    def cn(self, v):
        """Set the current columnnumber of this cursor."""
        symbols.cursor.set_cn(self.struct, v)

    @property
    def buffer(self):
        """Return the buffer this cursor is associated with."""
        return Buffer(symbols.cursor.get_buffer(self.struct))

    def move_cols(self, n):
        """Move this cursor forward n columns, or backwards -n columns."""
        symbols.cursor.move_cols(self.struct, n)

    def move_lines(self, n):
        """Move this cursor down n lines, or up -n lines."""
        symbols.cursor.move_lines(self.struct, n)

    def insert(self, string):
        """Insert a string at this cursor's current position. No newlines."""
        symbols.cursor.insert(self.struct, string)

    def delete(self, n):
        """Delete n characters from behind the cursor."""
        symbols.cursor.delete(self.struct, n)

    def enter(self):
        """Insert a newline at the cursor's current position."""
        symbols.cursor.enter(self.struct)

    def activate(self):
        """Activate this cursor. This is meant to be a cursor defined thing."""
        symbols.cursor.activate(self.struct)

    def deactivate(self):
        """Deactivate the cursor. Should perform the opposite of .activate()"""
        symbols.cursor.deactivate(self.struct)

    def open_stream(self):
        """Create and return a stream for this cursor to read bytes from.

        When the stream is written to, this cursor will insert whatever text
        is written to the stream. Writing newlines to the stream will call
        .enter on this cursor.
        """
        # Get a filepointer to the new stream.
        fptr = symbols.buffer.log.point_stream(self.struct)

        # Open the filepointer into a file.
        return cutil.fptr2file(fptr, "wb", 1)

class CursorContainer(Container):
    """A class to contain instances of cursor structs."""

    Obj           = CursorObj
    delete_struct = Hook(symbols.cursor.on_free, symbols.cursor.cursor_p)

Cursor = CursorContainer()

def initsys():
    """Initialize the cursor system.

    Should be called before any other cursor-based operations.
    """
    symbols.cursor.initsys()
    symbols.cursor.point.initsys()
    symbols.cursor.region.initsys()

def get_buffer_selected(buf):
    """Get the selected cursor in a specific buffer.

    Arguments:
        buf (Buffer): The buffer to find the selected cursor in.
    """
    return Cursor(symbols.cursor.buffer_selected(buf.struct))

def get_selected():
    """Get the current selected cursor.

    Equivilent to get_buffer_selected(core.windows.get_selected().buffer).
    """
    ptr = symbols.cursor.selected()

    if not ptr:
        return None

    return Cursor(ptr)

def spawn(buf, typ):
    """Spawn a cursor of a specific type in a buffer.

    Arguments:
       buf (Buffer): The buffer to spawn the new cursor in.
       typ (CursorType, CursorTypeFromPtr): The type of cursor to spawn.
    """
    return Cursor(symbols.cursor.spawn(buf.struct, typ.struct))

def select_next(buf):
    """Cause the next cursor in a buffer to be selected.

    This function will cycle through all cursors associated with a buffer before
    going back to the start.

    Arguments:
        buf (Buffer): The buffer to change the selected cursor in.
    """
    symbols.cursor.select_next(buf.struct)

def select_last(buf):
    """Causes the most recently created cursor in a buffer to be selected.

    Arguments:
        buf (Buffer): The buffer to change the selected cursor in.
    """
    symbols.cursor.select_last(buf.struct)

def delete_selected(buf):
    """Deletes the currently selected cursor in a buffer.

    Arguments:
        buf (Buffer): The buffer to delete the selected cursor in.
    """
    symbols.cursor.free(symbols.cursor.buffer_selected(buf.struct))

snap_blacklist = Vec(symbols.cursor.snap_blacklist,
                     symbols.cursor.cursor_type_p)
"""A vector of cursor types that the buffer should not scroll to."""

class types:
    """A class containing C-implemented cursor types."""

    point  = CursorTypeFromPtr(symbols.cursor.point.type)
    """A simple cursor type that acts as you'd expect a basic cursor to."""
    region = CursorTypeFromPtr(symbols.cursor.region.type)
    """A cursor type which can select and operate on a region when activated."""


class hooks:
    spawn           = Hook(symbols.cursor.on_spawn,
                           Cursor)
    free            = Hook(symbols.cursor.on_free,
                           Cursor)
    select          = Hook(symbols.cursor.on_select,
                           Buffer, Cursor)
    set_ln_pre      = Hook(symbols.cursor.on_set_ln_pre,
                           Cursor, symbols.buffer.lineno)
    set_ln_post     = Hook(symbols.cursor.on_set_ln_post,
                           Cursor, symbols.buffer.lineno)
    set_cn_pre      = Hook(symbols.cursor.on_set_cn_pre,
                           Cursor, symbols.buffer.colno)
    set_cn_post     = Hook(symbols.cursor.on_set_cn_post,
                           Cursor, symbols.buffer.colno)
    move_lines_pre  = Hook(symbols.cursor.on_move_lines_pre,
                           Cursor, ctypes.c_int)
    move_lines_post = Hook(symbols.cursor.on_move_lines_post,
                           Cursor, symbols.buffer.lineno, symbols.buffer.colno)
    move_cols_pre   = Hook(symbols.cursor.on_move_cols_pre,
                           Cursor, ctypes.c_int)
    move_cols_post  = Hook(symbols.cursor.on_move_cols_post,
                           Cursor, symbols.buffer.lineno, symbols.buffer.colno)
    insert_pre      = Hook(symbols.cursor.on_insert_pre,
                           Cursor, ctypes.c_char_p)
    insert_post     = Hook(symbols.cursor.on_insert_post,
                           Cursor, ctypes.c_char_p)
    delete_pre      = Hook(symbols.cursor.on_delete_pre,
                           Cursor, ctypes.c_uint)
    delete_post     = Hook(symbols.cursor.on_delete_post,
                           Cursor, ctypes.c_uint)
    enter_pre       = Hook(symbols.cursor.on_enter_pre,
                           Cursor)
    enter_post      = Hook(symbols.cursor.on_enter_post,
                           Cursor)
    change_pos      = Hook(symbols.cursor.on_change_pos,
                           Cursor, symbols.buffer.lineno, symbols.buffer.colno)

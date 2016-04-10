import symbols.win

import core.hook
import core.buffer
import ctypes
import cutil

class direction(symbols.win.dir):
    pass

def initsys():
    symbols.win.initsys()

def killsys():
    symbols.win.killsys()

def get_selected():
    return Window(symbols.win.select.get())

def get_root():
    return Window(symbols.win.root)

class Window:
    def __init__(self, ptr):
        self.valid  = True
        self.struct = ctypes.cast(ptr, symbols.win.win_p)

        @hooks.delete(900)
        def handle_delete(window):
            if window.struct == self.struct:
                window.valid = False
                self.valid   = False

        self.handle_delete = handle_delete

    @property
    def struct(self):
        if not self.valid:
            raise Exception("Invalid window")

        return self._struct

    @struct.setter
    def struct(self, value):
        self.valid   = True
        self._struct = value

    @property
    def size(self):
        return (symbols.win.size.get_x(self.struct),
                symbols.win.size.get_y(self.struct))

    @property
    def pos(self):
        return (symbols.win.pos.get_x(self.struct),
                symbols.win.pos.get_y(self.struct))

    @property
    def selected(self):
        return symbols.win.select.get() == self.struct

    def select(self):
        symbols.win.select.set(self.struct)

    @property
    def caption(self):
        return symbols.win.label.caption_get(self.struct)

    @caption.setter
    def caption(self, cap):
        if isinstance(cap, str):
            cap = cap.encode("ascii")

        symbols.win.label.caption_set(self.struct, cap)

    @property
    def sidebar(self):
        return symbols.win.label.sidebar_get(self.struct)

    @caption.setter
    def sidebar(self, sbar):
        if isinstance(sbar, str):
            sbar = sbar.encode("ascii")

        symbols.win.label.sidebar_set(self.struct, sbar)

    @property
    def buffer(self):
        return core.buffer.Buffer(symbols.win.get_buffer(self.struct))

    @property
    def parent(self):
        return Window(symbols.win.get_parent(self.struct))

    def delete(self):
        symbols.win.delete(self.struct)
        self.valid = False

    def split(self, direction):
        symbols.win.split(self.struct, direction)

    def __eq__(self, other):
        return self.struct == other.struct
    
    def __iter__(self):
        sub = symbols.win.get_first(self.struct)
        last = symbols.win.get_last(self.struct)

        while sub != last:
            yield Window(sub)
            sub = symbols.win.get_next(sub)

        yield Window(sub)

    def next(self):
        return Window(symbols.win.iter.next(self.struct))

class hooks:
    resize_x = core.hook.Hook(
        symbols.win.on_resize_x,
        Window,
        ctypes.c_uint,
        ctypes.c_uint)

    resize_y = core.hook.Hook(
        symbols.win.on_resize_y,
        Window,
        ctypes.c_uint,
        ctypes.c_uint)

    split = core.hook.Hook(
        symbols.win.on_split,
        Window,
        Window)

    delete = core.hook.Hook(
        symbols.win.on_delete,
        Window)

    create = core.hook.Hook(
        symbols.win.on_create,
        Window)

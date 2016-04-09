import c.win

import systems.hook
import ctypes
import cutil

class direction(c.win.dir):
    pass

def initsys():
    c.win.initsys()

def killsys():
    c.win.killsys()

def get_selected():
    return Window(c.win.select.get())

def get_root():
    return Window(c.win.root)

class Window:
    def __init__(self, ptr):
        self.valid  = True
        self.struct = ctypes.cast(ptr, c.win.win_p)

        @hooks.delete
        def handle_delete(window):
            if window == self:
                window.valid = False
                self.valid   = False

        self.handle_delete = handle_delete

    def __del__(self):
        hooks.delete.unmount(self.handle_delete)

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
        return (c.win.size.get_x(self.struct),
                c.win.size.get_y(self.struct))

    @property
    def pos(self):
        return (c.win.pos.get_x(self.struct),
                c.win.pos.get_y(self.struct))

    @property
    def selected(self):
        return c.win.select.get() == self.struct

    def select(self):
        c.win.select.set(self.struct)

    @property
    def caption(self):
        return c.win.label.caption_get(self.struct)

    @caption.setter
    def caption(self, cap):
        if isinstance(cap, str):
            cap = cap.encode("ascii")

        c.win.label.caption_set(self.struct, cap)

    @property
    def sidebar(self):
        return c.win.label.sidebar_get(self.struct)

    @caption.setter
    def sidebar(self, sbar):
        if isinstance(sbar, str):
            sbar = sbar.encode("ascii")

        c.win.label.sidebar_set(self.struct, sbar)

    @property
    def buffer(self):
        return c.win.get_buffer(self.struct)

    @property
    def parent(self):
        return Window(c.win.get_parent(self.struct))

    def delete(self):
        c.win.delete(self.struct)
        self.valid = False

    def split(self, direction):
        c.win.split(self.struct, direction)

    def __eq__(self, other):
        return self.struct == other.struct
    
    def __iter__(self):
        sub = c.win.get_first(self.struct)
        last = c.win.get_last(self.struct)

        while sub != last:
            yield Window(sub)
            sub = c.win.get_next(sub)

        yield Window(sub)

    def next(self):
        return Window(c.win.iter.next(self.struct))

class hooks:
    resize_x = systems.hook.Hook(
        c.win.on_resize_x,
        Window,
        ctypes.c_uint,
        ctypes.c_uint)

    resize_y = systems.hook.Hook(
        c.win.on_resize_y,
        Window,
        ctypes.c_uint,
        ctypes.c_uint)

    split = systems.hook.Hook(
        c.win.on_split,
        Window,
        Window)

    delete = systems.hook.Hook(
        c.win.on_delete,
        Window)

    create = systems.hook.Hook(
        c.win.on_create,
        Window)

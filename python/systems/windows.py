import c.win

import systems.hook
import ctypes
import cutil

class direction(c.wintree.dir):
    pass

class hooks:
    pass

def initsys():
    c.win.initsys()

def killsys():
    c.win.killsys()

def get_selected(self):
    return Window(c.win.select.get())

def get_root():
    return Window(c.win.root)

class Window:
    def __init__(self, ptr):
        self.valid  = True
        self.struct = ctypes.cast(ptr, win.win_p)

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
        c.win.label.caption_set(cap)

    @property
    def sidebar(self):
        return c.win.label.sidebar_get(self.struct)

    @caption.setter
    def sidebar(self, sbar):
        c.win.label.caption_set(sbar)

    @property
    def parent(self):
        return Window(self.struct)

    def delete(self):
        c.win.delete(self.struct)
        self.valid = False

    def split(self, direction):
        return 0

    def __iter__(self):
        sub = c.win.get_first(self.struct)
        last = c.win.get_last(self.struct)

        while sub != last:
            yield Window(sub)
            sub = c.win.get_next(sub)

        yield Window(sub)

    def next(self):
        return Window(c.win.get_next(self.struct))

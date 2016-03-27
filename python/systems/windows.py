import c.wincont
import c.wintree
import c.ui

import systems.hook
import ctypes
import cutil

class direction(c.wintree.dir):
    pass

class hooks:
    resizex = None
    resizey = None
    delete  = None
    create  = None

def initsys():
    c.wincont.initsys()
    c.wintree.initsys()

    hooks.resizex = systems.hook.Hook(c.wintree.on_resizex,
                                      window, ctypes.c_size_t, ctypes.c_size_t)

    hooks.resizey = systems.hook.Hook(c.wintree.on_resizey,
                                      window, ctypes.c_size_t, ctypes.c_size_t)

    hooks.delete = systems.hook.Hook(c.wintree.on_delete,
                                     window)

    hooks.create = systems.hook.Hook(c.wintree.on_create,
                                     window)

def split(direction):
    c.wintree.split(c.wintree.get_selected(), direction)

def select_next():
    curr = c.wintree.get_selected()
    next = c.wintree.iter_next(curr)

    if cutil.isnull(next):
        next = c.wintree.iter_start()

    c.wintree.select(next)

def select_up():
    curr = c.wintree.get_selected()
    parent = c.wintree.get_parent(curr)
    c.wintree.select(parent)

def delete():
    c.wintree.delete(c.wintree.get_selected())

class window:
    def __init__(self, ptr):
        self.struct = ctypes.cast(ptr, ctypes.POINTER(c.wintree.wintree_p)).contents


    @property
    def caption(self):
        return c.wintree.get_caption(self.struct)

    @caption.setter
    def caption(self, value):
        if isinstance(value, str):
            value = bytes(value, "ascii")

        c.wintree.set_caption(self.struct, value)

    @property
    def sidebar(self):
        return c.wintree.get_sidebar(self.struct)

    @sidebar.setter
    def sidebar(self, value):
        if isinstance(value, str):
            value = bytes(value, "ascii")

        c.wintree.set_sidebar(self.struct, value)

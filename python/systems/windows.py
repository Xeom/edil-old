import c.wincont
import c.wintree

import systems.hook
import ctypes
import cutil

class direction(c.wintree.dir):
    pass

class hooks:
    resize = None
    delete = None
    create = None

def initsys():
    c.wincont.initsys()
    c.wintree.initsys()

    hooks.resizex = systems.hook.Hook(c.wintree.on_resizex,
                                      c.wintree.wintree_p, ctypes.c_size_t, ctypes.c_size_t)

    hooks.resizey = systems.hook.Hook(c.wintree.on_resizey,
                                      c.wintree.wintree_p, ctypes.c_size_t, ctypes.c_size_t)

    hooks.delete = systems.hook.Hook(c.wintree.on_delete,
                                     c.wintree.wintree_p)

    hooks.create = systems.hook.Hook(c.wintree.on_create,
                                     c.wintree.wintree_p)

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

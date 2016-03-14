import c.wincont
import c.wintree

import systems.hook
import ctypes

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
    c.wintree.select_next(c.wintree.get_selected())

def select_up():
    c.wintree.select_up(c.wintree.get_selected())

def delete():
    c.wintree.delete(c.wintree.get_selected())

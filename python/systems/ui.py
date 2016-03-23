import c.ui
import c.wintree
import c.lib
import ctypes

import systems.hook
import systems.callback

class callbacks:
    pass

class hooks:
    resize = None

def initsys():
    c.ui.initsys()
    c.ui.resize()

    hooks.resize = systems.hook.Hook(c.ui.on_resize,
                                      ctypes.c_size_t, ctypes.c_size_t)

def resize():
    c.ui.resize()
    refresh()

def refresh():
    c.lib.clear()
    c.ui.refresh()

def killsys():
    c.ui.killsys()

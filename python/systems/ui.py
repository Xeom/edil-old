import c.ui
import c.lib
import ctypes

import systems.hook

class hooks:
    resize = None

def initsys():
    c.ui.initsys()
    c.ui.resize()

    hooks.resize = systems.hook.Hook(c.ui.on_resize,
                                     ctypes.c_size_t, ctypes.c_size_t)

def resize():
    import sys
    c.ui.resize()
    sys.stderr.write("Did resize\n")
    refresh()

def refresh():
    c.lib.clear()
    c.ui.display_wintrees()
    c.lib.refresh()

def killsys():
    c.lib.endwin()
    c.ui.killsys()

def resize_handle(x, y):
    open("resizes.txt", "a").write("{} {}\n".format(x, y))

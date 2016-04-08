import c.ui
import c.lib
import c.vec
import c.buffer
import ctypes

import systems.windows
import systems.hook
import systems.callback

class callbacks:
    pass

class hooks:
    resize = systems.hook.Hook(
        c.ui.on_resize,
        ctypes.c_size_t,
        ctypes.c_size_t)

    class win:
        class frame:
            draw_pre  = systems.hook.Hook(
                c.ui.win.frame.on_draw_pre,
                systems.windows.Window)

            draw_post = systems.hook.Hook(
                c.ui.win.frame.on_draw_post,
                systems.windows.Window)

        class content:
            draw_pre  = systems.hook.Hook(
                c.ui.win.content.on_draw_pre,
                systems.windows.Window,
                c.buffer.buffer_p)

            draw_post = systems.hook.Hook(
                c.ui.win.content.on_draw_post,
                systems.windows.Window,
                c.buffer.buffer_p)

            draw_line_pre  = systems.hook.Hook(
                c.ui.win.content.on_draw_line_pre,
                systems.windows.Window,
                c.buffer.buffer_p,
                c.buffer.lineno,
                c.vec.VecType(ctypes.c_char))

            draw_line_post = systems.hook.Hook(
                c.ui.win.content.on_draw_line_post,
                systems.windows.Window,
                c.buffer.buffer_p,
                c.buffer.lineno,
                c.vec.VecType(ctypes.c_char))

def initsys():
    c.ui.initsys()
    c.ui.resize()

def resize():
    c.ui.resize()
    refresh()

def refresh():
    c.lib.clear()
    c.ui.refresh()

def killsys():
    c.ui.killsys()

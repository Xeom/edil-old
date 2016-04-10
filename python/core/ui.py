import symbols.ui
import symbols.lib
import symbols.vec
import symbols.buffer
import ctypes

import core.buffer
import core.windows
import core.hook
import core.callback

class callbacks:
    pass

class hooks:
    resize = core.hook.Hook(
        symbols.ui.on_resize,
        ctypes.c_size_t,
        ctypes.c_size_t)

    class win:
        class frame:
            draw_pre  = core.hook.Hook(
                symbols.ui.win.frame.on_draw_pre,
                core.windows.Window)

            draw_post = core.hook.Hook(
                symbols.ui.win.frame.on_draw_post,
                core.windows.Window)

        class content:
            draw_pre  = core.hook.Hook(
                symbols.ui.win.content.on_draw_pre,
                core.windows.Window,
                core.buffer.Buffer)

            draw_post = core.hook.Hook(
                symbols.ui.win.content.on_draw_post,
                core.windows.Window,
                core.buffer.Buffer)

            draw_line_pre  = core.hook.Hook(
                symbols.ui.win.content.on_draw_line_pre,
                core.windows.Window,
                core.buffer.Buffer,
                symbols.buffer.lineno,
                symbols.vec.Vec.Type(ctypes.c_char))

            draw_line_post = core.hook.Hook(
                symbols.ui.win.content.on_draw_line_post,
                core.windows.Window,
                core.buffer.Buffer,
                symbols.buffer.lineno,
                symbols.vec.Vec.Type(ctypes.c_char))

def initsys():
    symbols.ui.initsys()
    symbols.ui.resize()

def resize():
    symbols.ui.resize()
    refresh()

def refresh():
    symbols.lib.clear()
    symbols.ui.refresh()

def killsys():
    symbols.ui.killsys()

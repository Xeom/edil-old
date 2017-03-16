import symbols.ui
import symbols.lib
import symbols.vec
import symbols.buffer
import ctypes
import signal

import core.buffer
import core.windows
import core.hook

from core.vec     import Vec
from core.windows import Window
from core.buffer  import Buffer
from core.hook    import Hook

class hooks:
    resize_pre =  Hook(
        symbols.ui.on_resize_pre,
        ctypes.c_size_t,
        ctypes.c_size_t)
    resize_post = Hook(
        symbols.ui.on_resize_post,
        ctypes.c_size_t,
        ctypes.c_size_t)

    class win:
        class frame:
            draw_pre  = Hook(symbols.ui.win.frame.on_draw_pre,
                             Window)

            draw_post = Hook(symbols.ui.win.frame.on_draw_post,
                             Window)

        class content:
            draw_pre  = Hook(symbols.ui.win.content.on_draw_pre,
                             Window,
                             Buffer)

            draw_post = Hook(symbols.ui.win.content.on_draw_post,
                             Window,
                             Buffer)

            draw_line_pre  = Hook(symbols.ui.win.content.on_draw_line_pre,
                                  Window,
                                  Buffer,
                                  symbols.buffer.lineno,
                                  Vec.Type(ctypes.c_char))

            draw_line_post = Hook(symbols.ui.win.content.on_draw_line_post,
                                  Window,
                                  Buffer,
                                  symbols.buffer.lineno,
                                  Vec.Type(ctypes.c_char))

cursorface = symbols.ui.face.cursor.value

def updates_hold(b):
    symbols.ui.updates.hold(b.struct)

def updates_release():
    symbols.ui.updates.release()

def initsys():
    symbols.ui.initsys()
    symbols.ui.resize()

def resize():
    symbols.ui.resize()
    refresh()

def refresh():
    symbols.lib.clear()
    symbols.ui.refresh()

def nc_refresh():
    symbols.lib.refresh()

def killsys():
    symbols.ui.killsys()

def draw_window_line(w, ln):
    symbols.ui.win.content.draw_line(w.struct, ln)

def draw_buffer_line(b, ln):
    for w in core.windows.get_root():
        if w.buffer == b:
            symbols.ui.win.content.draw_line(w.struct, ln)

def draw_buffer(b):
    for w in core.windows.get_root():
        if w.buffer == b:
            symbols.ui.win.content.draw(w.struct)

def draw_buffer_after(b, ln):
    for w in core.windows.get_root():
        if w.buffer == b:
            symbols.ui.win.content.draw_lines_after(w.struct, ln)

def set_sbar(s):
    if isinstance(s, str):
        s = s.encode("ascii")

    symbols.ui.sbar.set(s)
    symbols.ui.sbar.draw()

def get_sbar():
    return symbols.ui.sbar.content.value

def draw_window(w):
    symbols.ui.win.draw(w.struct)

def draw_window_frame(w):
    symbols.ui.win.frame.draw_subs(w.struct)

def get_window_cursor_posx(w, cur):
    return symbols.ui.win.content.get_cursor_offset(w.struct, cur.ln)

def text_next_char(string):
    start = ctypes.c_char_p(string)
    end   = ctypes.cast(start, ctypes.c_void_p).value + len(string)
    end   = ctypes.cast(end, ctypes.c_char_p)

    rtn = symbols.ui.util.text_next_char(start, end)

    if rtn == None:
        return None

    else:
        return bytes(rtn)

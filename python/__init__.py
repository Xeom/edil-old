#!/bin/python

import core.ui
import core.windows
import core.hook
import core.io
import time
import ctypes
import shared
import signal
import symbols
from editor.cursor.point import Point


#@core.ui.hooks.win.frame.draw_pre(100)
#def handle_frame_draw_pre(w):
#    w.caption = str(len(w.buffer))
#
#@core.ui.hooks.win.content.draw_pre(100)
#def handle_content_draw_pre(w, b):
#    b.push("I was drawn :D")
#
#@core.windows.hooks.split(100)
#def handle_split(wb, wbaby):
#    b = wbaby.buffer
#
#@core.windows.hooks.create(100)
#def handle_created(w):
#    b = w.buffer

core.windows.initsys()
print("HI")
core.ui.initsys()
core.io.initsys()

core.ui.refresh()

p = Point(core.windows.get_selected().buffer)

while True:
    char = symbols.lib.getch()

    if char == symbols.io.key_resize:
        core.ui.resize()

    elif char == ord('w'):
        core.windows.get_selected().split(core.windows.direction.up)

    elif char == ord('s'):
        core.windows.get_selected().split(core.windows.direction.down)

    elif char == ord('a'):
        core.windows.get_selected().split(core.windows.direction.left)

    elif char == ord('d'):
        core.windows.get_selected().split(core.windows.direction.right)

    elif char == ord('l'):
        core.windows.get_selected().buffer.read("__init__.py")

    elif char == ord('e'):
        core.windows.get_selected().next().select()

    elif char == ord('c'):
        core.windows.get_selected().delete()

    elif char == ord('x'):
        break

    elif char == ord('u'):
        core.windows.get_selected().parent.select()

    elif char == 10:
        p.enter()

    else:
        p.insert(chr(char))
#        symbols.io.handle_chr(char)

#k    core.windows.Window(symbols.win.select.get()).sidebar = "Hiiii"
    core.ui.refresh()

core.ui.killsys()


#!/bin/python

import systems.ui
import systems.windows
import systems.hook
import systems.io
import c
import time
import ctypes
import shared
import signal
import c.lib

#shared.lib.err_initsys()
systems.windows.initsys()

systems.ui.initsys()
systems.io.initsys()
systems.ui.refresh()

open("textlog.txt", "w").write("")

@systems.windows.hooks.resizex(100)
@systems.windows.hooks.resizey(100)
def handle_resize(w, x, y):
    sizex = x.value
    sizey = y.value

    w.sidebar = "Cunt"

    w.caption = "{}x{}".format(sizex, sizey)

@systems.io.hooks.keypress(1)
def handle_hook(k):
    c.ui.sbar.set(bytes(k))
    c.lib.refresh()

while True:
    char = c.lib.getch()

    if char == c.io.key_resize:
        systems.ui.resize()

    if char == ord('w'):
        systems.windows.split(systems.windows.direction.up)

    if char == ord('s'):
        systems.windows.split(systems.windows.direction.down)

    if char == ord('a'):
        systems.windows.split(systems.windows.direction.left)

    if char == ord('d'):
        systems.windows.split(systems.windows.direction.right)

    if char == ord('e'):
        systems.windows.select_next()

    if char == ord('c'):
        systems.windows.delete()

    if char == ord('x'):
        break

    if char == ord('u'):
        systems.windows.select_up()

    else:
        c.io.handle_chr(char)

    c.lib.refresh()

systems.ui.killsys()


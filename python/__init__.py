#!/bin/python

import systems.ui
import systems.windows
import systems.faces
import systems.hook
import systems.io
import c.lib
import c.ui
import time

import shared
import signal

#shared.lib.err_initsys()
systems.windows.initsys()
systems.ui.initsys()
systems.faces.initsys()
systems.io.initsys()

systems.ui.refresh()

open("textlog.txt", "w").write("")

@systems.io.hooks.keypress(1)
def handle_hook(k):
    s = c.io.key_str(k)
    c.ui.set_statusbar(s)
    c.lib.refresh()

while True:
    char = c.lib.getch()

    if char == c.ui.key_resize:
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
    systems.ui.refresh()

sizex, sizey = c.wintree.get_sizex(c.wintree.get_selected()), c.wintree.get_sizey(c.wintree.get_selected())

systems.ui.killsys()
print(sizex, sizey)

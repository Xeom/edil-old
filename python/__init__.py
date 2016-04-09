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

def append(b, text):
    c.buffer.insert(b, 0)

    l = c.vec.str2vec(text)

    c.buffer.set_line(b, 0, l.struct)


@systems.ui.hooks.win.content.draw_pre(100)
def handle_content_draw_pre(w, b):
    append(b, b"I was drawn ^-^")

    if not w.caption:
        w.caption = "0"

    w.caption = str(int(w.caption) + 1)

@systems.windows.hooks.split(100)
def handle_split(wb, wbaby):
    b = wbaby.buffer
    append(b, b"I was created in a split :D")

@systems.windows.hooks.create(100)
def handle_created(w):
    b = w.buffer
    append(b, b"Yay I exist!!")

systems.windows.initsys()
systems.ui.initsys()
systems.io.initsys()

systems.ui.refresh()



import c.buffer


"""
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
"""



while True:
    char = c.lib.getch()

    if char == c.io.key_resize:
        systems.ui.resize()

    if char == ord('w'):
        systems.windows.get_selected().split(systems.windows.direction.up)

    if char == ord('s'):
        systems.windows.get_selected().split(systems.windows.direction.down)

    if char == ord('a'):
        systems.windows.get_selected().split(systems.windows.direction.left)

    if char == ord('d'):
        systems.windows.get_selected().split(systems.windows.direction.right)

    if char == ord('e'):
        systems.windows.get_selected().next().select()

    if char == ord('c'):
        systems.windows.get_selected().delete()

    if char == ord('x'):
        break

    if char == ord('u'):
        systems.windows.get_selected().parent.select()

    else:
        c.io.handle_chr(char)

#k    systems.windows.Window(c.win.select.get()).sidebar = "Hiiii"
    systems.ui.refresh()

systems.ui.killsys()

import gc
gc.collect()

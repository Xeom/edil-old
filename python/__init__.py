#!/bin/python

import core.ui
import core.windows
import core.hook
import core.key
import ctypes
import shared
import symbols

import editor.uiupdates
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
core.ui.initsys()
core.key.initsys()

core.ui.refresh()
alive = True
@core.key.hooks.key(100)
def handle_keypress(key):
    char = str(key)

    if char == '^W':
        core.windows.get_selected().split(core.windows.direction.up)

    elif char == '^S':
        core.windows.get_selected().split(core.windows.direction.down)

    elif char == '^A':
        core.windows.get_selected().split(core.windows.direction.left)

    elif char == '^D':
        core.windows.get_selected().split(core.windows.direction.right)

    elif char == '^L':
        core.windows.get_selected().buffer.read("__init__.py")

    elif char == '^E':
        core.windows.get_selected().next().select()

    elif char == '^C':
        core.windows.get_selected().delete()

    elif char == '^X':
        global alive
        alive = False

    elif char == '^U':
        core.windows.get_selected().parent.select()

    elif char == '^J':
        p.enter()

    elif char == 'BACKSPACE':
        p.delete_char()

    else:
        p.insert(char)

p = Point(core.windows.get_selected().buffer)

while alive:
    char = symbols.lib.getch()

    if char == symbols.io.key.resize:
        core.ui.resize()

    else:
        symbols.io.key.handle_chr(char)

#    core.ui.refresh()

#k    core.windows.Window(symbols.win.select.get()).sidebar = "Hiiii"

core.ui.killsys()


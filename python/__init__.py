#!/bin/python

import core.ui
import core.windows
import core.hook
import core.key
import core.keymap
import ctypes
import shared
import symbols
import signal

import editor.uiupdates
from editor.cursor.point import Point
from core.key import Key
from core.keymap import Keymap

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
symbols.hook.initsys()

core.windows.initsys()
core.ui.initsys()
core.key.initsys()

core.ui.refresh()

@core.key.hooks.key(100)
def handle_keypress(key):
    km.press(key)
    return
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

    elif char == '^F':
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

p  = Point(core.windows.get_selected().buffer)
km = core.keymap.Keymap()
km.struct

alive = True

def death_by_gentle_suffocation( signum, frame):
    global alive
    alive = False

signal.signal(signal.SIGINT, death_by_gentle_suffocation)
signal.signal(signal.SIGTERM, death_by_gentle_suffocation)

@km.add(Key("W", con=True), Key("UP"))
def winsplitup(keys):
    core.windows.get_selected().split(core.windows.direction.up)

@km.add(Key("W", con=True), Key("DOWN"))
def winsplitdown(keys):
    core.windows.get_selected().split(core.windows.direction.down)

@km.add(Key("W", con=True), Key("LEFT"))
def winsplitleft(keys):
    core.windows.get_selected().split(core.windows.direction.left)

@km.add(Key("W", con=True), Key("RIGHT"))
def winsplitright(keys):
    core.windows.get_selected().split(core.windows.direction.right)

    
while alive:
    char = symbols.lib.getch()

    if char == symbols.io.key.resize:
        core.ui.resize()

    else:
        symbols.io.key.handle_chr(char)

#    core.ui.refresh()

#k    core.windows.Window(symbols.win.select.get()).sidebar = "Hiiii"

core.ui.killsys()

#symbols.hook.killsys()

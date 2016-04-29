#!/bin/python

import core.ui
import core.windows
import core.hook
import core.key
import core.keymap
import core.table
import ctypes
import shared
import symbols
import signal

symbols.hook.initsys()
core.windows.initsys()
core.ui.initsys()
core.key.initsys()

core.ui.refresh()

import editor.uiupdates

import editor.keymap.keymap

from editor.subcaption   import SubCaption
from core.key import Key
from core.keymap import Keymap

alive = True

def death_by_gentle_suffocation(signum, frame):
    global alive
    alive = False

signal.signal(signal.SIGINT,  death_by_gentle_suffocation)
signal.signal(signal.SIGTERM, death_by_gentle_suffocation)

@SubCaption
def getsize(win):
    x, y = win.size

    return "{}x{}".format(x, y)

@SubCaption
def getselected(win):
    if win.selected:
        return "*"

    return ""

mastermap = core.keymap.maps["master"]

@mastermap.add(Key("x", con=True))
def masterexit(keys):
    global alive
    alive = False

while alive:
    char = symbols.lib.getch()

    if char == symbols.io.key.resize:
        core.ui.resize()

    else:
        symbols.io.key.handle_chr(char)

core.ui.killsys()

symbols.hook.killsys()

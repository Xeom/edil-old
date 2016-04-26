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
from editor.cursor.point import Point
from core.key import Key
from core.keymap import Keymap

alive = True

def death_by_gentle_suffocation( signum, frame):
    global alive
    alive = False

signal.signal(signal.SIGINT,  death_by_gentle_suffocation)
signal.signal(signal.SIGTERM, death_by_gentle_suffocation)

p  = Point(core.windows.get_selected().buffer)

################################################################################
core.keymap.maps.add("win")

winmap = core.keymap.maps["win"]

@winmap.add(Key("s"), Key("UP"))
def winsplitup(keys):
    core.windows.get_selected().split(core.windows.direction.up)

@winmap.add(Key("s"), Key("DOWN"))
def winsplitdown(keys):
    core.windows.get_selected().split(core.windows.direction.down)

@winmap.add(Key("s"), Key("LEFT"))
def winsplitleft(keys):
    core.windows.get_selected().split(core.windows.direction.left)

@winmap.add(Key("s"), Key("RIGHT"))
def winsplitright(keys):
    core.windows.get_selected().split(core.windows.direction.right)

@winmap.add(Key("x"))
def windel(keys):
    core.windows.get_selected().delete()

@winmap.add(Key("RIGHT"))
def winnext(keys):
    core.windows.get_selected().next().select()

@winmap.add(Key("LEFT"))
def winprev(keys):
    core.windows.get_selected().prev().select()

#@winmap.add(Key("UP"))
#def winup(keys):
#    core.windows.get_selected().parent.select()
#
##@winmap.add(Key("DOWN",  esc=True))
#@winmap.add(Key("RIGHT", esc=True))
#def winadjplus(keys):
#    core.windows.get_selected().adj(1)
#
##@winmap.add(Key("UP",   esc=True))
#@winmap.add(Key("LEFT", esc=True))
#def winadjminus(keys):
#    core.windows.get_selected().adj(-1)
#
core.keymap.maps.add("master")
mastermap = core.keymap.maps["master"]

@mastermap.add(Key("W", con=True))
def masterwinon(keys):
    core.keymap.frames.switch("winf")

@winmap.add(Key("w", esc=True))
def masterwinoff(keys):
    core.keymap.frames.switch("masterf")

@mastermap.add(Key("x"))
def masterexit(keys):
    global alive
    alive = False

core.keymap.frames.add("masterf")
core.keymap.frames.add("winf")
core.keymap.frames["masterf"].push("master")
core.keymap.frames["winf"].push("win")
core.keymap.frames.switch("masterf")

#################################################################################

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

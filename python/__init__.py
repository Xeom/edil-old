#!/bin/python
#import cProfile, pstats, io
#pr = cProfile.Profile()
#pr.enable()

import core.ui
import core.windows
import core.hook
import core.key
import core.keymap
import core.table
import core.buffer
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

import editor.cursor.regioncursor
import editor.cursor.cursor
import editor.keymap.keymap
import editor.clipboard
import editor.files

from editor.subcaption   import SubCaption
from core.key import Key
from core.keymap import Keymap
from core.face   import Face

alive = True

def death_by_gentle_suffocation(signum, frame):
    global alive
    alive = False

signal.signal(signal.SIGINT,  death_by_gentle_suffocation)
signal.signal(signal.SIGTERM, death_by_gentle_suffocation)

@SubCaption(core.windows.hooks.size.adj_post,
            core.windows.hooks.split,
            core.windows.hooks.create)
def getsize(win):
    x, y = win.size

    return "{}x{}".format(x, y)

@SubCaption(core.windows.hooks.select)
def getselected(win):
    if win.selected:
        return "*"

    return ""

@core.ui.hooks.win.content.draw_line_pre(800)
def addlineno(w, b, ln, v):
    face = Face(Face.black, Face.black, bright=True)
    string =  hex(ln.value)[2:].zfill(4) + " "
    string = face.serialize(len(string)) + string.encode("ascii")

    v.insert_bytes(0, string)

mastermap = core.keymap.maps["master"]
@mastermap.add(Key("V", con=True))
def paste(keys):
    editor.clipboard.do_paste(editor.cursor.cursor.cursors.current)

@mastermap.add(Key("X", con=True))
def masterexit(keys):
    global alive
    alive = False

@mastermap.add(Key("F", con=True), Key("a"))
def masterload(keys):
    b = core.windows.get_selected().buffer
    c = editor.cursor.cursor.cursors.current

    fn = bytes(b[c.ln])


    editor.files.associate(b, fn)

@mastermap.add(Key("F", con=True), Key("s"))
def mastersave(keys):
    editor.files.dump(
        core.windows.get_selected().buffer)

while alive:
    char = symbols.lib.getch()

    if char == symbols.io.key.resize:
        core.ui.resize()

    else:
        symbols.io.key.handle_chr(char)

core.ui.killsys()

#symbols.hook.killsys()
#pr.disable()
#s = io.StringIO()
#sortby = 'cumulative'
#ps = pstats.Stats(pr, stream=s).sort_stats(sortby)
#ps.print_stats()
#import sys
#print(s.getvalue(), file=sys.stderr)

#!/bin/python
import cProfile, pstats, io, select, sys
pr = cProfile.Profile()
pr.enable()

import core.ui
import core.windows
import core.hook
import core.key
import core.keymap
import core.table
import core.buffer
import core.deferline
import core.point
import ctypes
import shared
import symbols
import signal

symbols.hook.initsys()
core.windows.initsys()
core.deferline.initsys()
core.ui.initsys()
core.key.initsys()
core.point.initsys()
core.ui.refresh()
symbols.buffer.log.initsys()
symbols.io.listener.initsys()
#import editor.uiupdates

import editor.cursor.regioncursor
import editor.cursor.cursor
import editor.keymap.keymap
import editor.clipboard
import editor.files
import editor.buffers.ring

ctypes.cast(shared.lib.err_stream, ctypes.POINTER(ctypes.c_void_p)).contents.value = symbols.buffer.log.stream()

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

@core.deferline.hooks.draw(200)
def addlineno(w, b, ln, li):
    face = Face(Face.black, Face.black, bright=True)
    prefix =  hex(ln.value)[2:].zfill(4) + "> "
    prefix = face.serialize(len(prefix)) + prefix.encode("ascii")

    li.insert(0, prefix)

@core.deferline.hooks.draw(400)
def addeol(w, b, ln, li):
    face = Face(Face.black, Face.black, bright=True)
    suffix = "<"
    suffix = b" " + face.serialize(len(suffix)) + suffix.encode("ascii")

    li.insert(len(li.vec), suffix)

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
    symbols.io.listener.listen()
    continue
    char = symbols.lib.getch()

    if char == symbols.io.key.resize:
        core.ui.resize()

    else:
        symbols.io.key.handle_chr(char)

core.ui.killsys()

#symbols.hook.killsys()
pr.disable()

sortby = 'tottime'
import sys
ps = pstats.Stats(pr, stream=sys.stderr)
ps.strip_dirs()
ps.sort_stats(sortby)
#ps.print_stats(15)
#ps.print_callers('cast')


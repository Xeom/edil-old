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
symbols.io.listener.initsys()
core.windows.initsys()
core.buffer.initsys()
core.deferline.initsys()
core.ui.initsys()
core.key.initsys()
core.keymap.initsys()
core.point.initsys()
core.ui.refresh()
symbols.buffer.log.initsys()

import editor.query
import editor.cursor.regioncursor
import editor.cursor.cursor
import editor.clipboard
import editor.files
import editor.buffers.ring
import editor.bind.keymap
import editor.autocomplete


from editor.cursor.cursor import cursors

#shared.lib.err_create_log_buffer()
#ctypes.cast(shared.lib.err_stream, ctypes.POINTER(ctypes.c_void_p)).contents.value = symbols.buffer.log.stream()

from editor.command      import Command, CommandArg
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
    editor.clipboard.do_paste(cursors.current)

@mastermap.add(Key("X", con=True))
def masterexit(keys):
    global alive
    alive = False

cmd = Command(CommandArg(int, "n ", editor.autocomplete.number()),
              CommandArg(str, "string "))

@cmd.hook(500)
def queryish_cb(n, string):
    cursors.current.insert(string * n)

@mastermap.add(Key("R", con=True), Key("e"), Key("p"))
def queryish(keys):
    cmd.run()

@core.key.hooks.key(500)
def hi(key):
    print(key, file=sys.stderr)

import editor.userlog

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
ps.print_stats(15)
ps.print_callers('cast')


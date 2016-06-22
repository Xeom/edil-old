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
import core.cursor
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
core.cursor.initsys()
symbols.buffer.log.initsys()

import editor.query
import editor.clipboard
import editor.files
import editor.buffers.ring
import editor.buffers.userlog
import editor.bind.keymap
import editor.autocomplete

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

@core.deferline.hooks.draw(50)
def addlineno(w, b, ln, li):
    face = Face(Face.black, Face.black, bright=True)
    prefix =  hex(ln.value)[2:].zfill(4) + "> "
    prefix = face.serialize(len(prefix)) + prefix.encode("ascii")

    li.insert(0, prefix)

@core.deferline.hooks.draw(900)
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

cmd = Command("repeat-string", CommandArg(int, "n" , editor.autocomplete.number()),
              CommandArg(str, "string"))

@cmd.hook(500)
def queryish_cb(n, string):
    cursors.current.insert(string * n)

@mastermap.add(Key("R", con=True), Key("e"), Key("p"))
def queryish(keys):
    cmd.run()

scroll_selected_face = Face(Face.white, Face.black)
@core.windows.hooks.offsety_set(700)
def handle_offsety_set(win, old):
    total  = len(win.buffer)
    before = win.offsety
    seen   = min(total - before, win.textsize[1])
    after  = max(0, total - seen - before)

    before = round(before * win.textsize[1] / total)
    after  = round(after  * win.textsize[1] / total)
    seen   = win.textsize[1] - before - after

    win.sidebar = (b" " * before) + (b"|" * seen) + (b" " * after)

@core.key.hooks.key(500)
def hi(key):
    print(key, file=sys.stderr)

cur = core.cursor.spawn(core.windows.get_selected().buffer,
                        core.cursor.types.region)
cur.insert(b"SLUT")
cur.enter()
cur.insert(b"I love you really <3")

while alive:
    symbols.io.listener.listen()

core.ui.killsys()

#symbols.hook.killsys()
pr.disable()

sortby = 'tottime'
import sys
ps = pstats.Stats(pr, stream=sys.stderr)
ps.strip_dirs()
ps.sort_stats(sortby)
ps.print_stats(30)
ps.print_callers('__iter__')


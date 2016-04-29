import string

import core.keymap

from core.key import Key
from editor.cursor.cursor import cursors

mapname = "cursor-default"

core.keymap.maps.add(mapname)
curmap = core.keymap.maps[mapname]

@curmap.add(Key("UP"))
def cur_up(keys):
    cursors.current.move_lines(-1)

@curmap.add(Key("DOWN"))
def cur_down(keys):
    cursors.current.move_lines(1)

@curmap.add(Key("LEFT"))
def cur_left(keys):
    cursors.current.move_cols(-1)

@curmap.add(Key("RIGHT"))
def cur_right(keys):
    cursors.current.move_cols(1)

@curmap.add(Key("BACKSPACE"))
def cur_delete(keys):
    cursors.current.delete_char()

@curmap.add(Key("A", con=True))
def cur_activate(keys):
    cursors.current.activate()

@curmap.add(Key("A", esc=True))
def cur_deactivate(keys):
    cursors.current.deactivate()

import sys

@curmap.add(Key("J", con=True))
def cur_enter(keys):
    cursors.current.enter()

def cur_insert(keys):
    if len(keys) != 1:
        raise NO

    kstr = bytes(keys[0])

    if len(kstr) != 1:
        raise NO

    cursors.current.insert_char(kstr)

for char in ("!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~ "
             "abcdefghijklmnopqrstuvwxyz"
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "0123456789"):
    curmap.add(Key(char))(cur_insert)

import string

import core.keymap
import editor.buffers.userlog

from core.key import Key
from editor.cursor.cursor import cursors
from editor.command import Command, CommandArg

mapname = "cursor-default"

core.keymap.maps.add(mapname)
curmap = core.keymap.maps[mapname]

# cursor-up
#
# Moves the cursor down a specified number of lines.
# Negative values move the cursor up. Called with
# argument 1 by UP key.

up_cmd = Command("cursor-up",
                 CommandArg(int, "Lines to move up"))
@up_cmd.hook(500)
def up_cb(n):
    cursors.current.move_lines(-n)

@curmap.add(Key("UP"))
def up_mapped(keys):
    up_cmd.run_withargs(1)

# cursor-down
#
# Moves the cursor down a specified number of lines.
# Negative values move the cursor down. Called with
# argument 1 by DOWN key.

down_cmd = Command("cursor-down",
                   CommandArg(int, "Lines to move down"))
@down_cmd.hook(500)
def down_cb(n):
    cursors.current.move_lines(n)

@curmap.add(Key("DOWN"))
def down_mapped(keys):
    down_cmd.run_withargs(1)

# cursor-back
#
# Moves the cursor back in its buffer a specified
# number of characters. If the cursor reaches the
# start of a line, it goes to the previous line.
# Called with argument 1 by the LEFT key.

back_cmd = Command("cursor-back",
                   CommandArg(int, "Characters to move back"))
@back_cmd.hook(500)
def back_cb(n):
    cursors.current.move_cols(-n)

@curmap.add(Key("LEFT"))
def back_mapped(keys):
    back_cmd.run_withargs(1)

# cursor-forward
#
# Moves the cursor forward in its buffer a specified
# number of characters. If the cursor reaches the
# end of a line, it goes to the end line. Called
# with argument 1 by the RIGHT key.

forward_cmd = Command("cursor-back",
                      CommandArg(int, "Characters to move forward"))
@forward_cmd.hook(500)
def forward_cb(n):
    cursors.current.move_cols(n)

@curmap.add(Key("RIGHT"))
def forward_mapped(keys):
    forward_cmd.run_withargs(1)

# cursor-delback
#
# Deletes a specified number of characters behind the
# cursor. Called with argument 1 by the BACK key.

delback_cmd = Command("cursor-delback",
                      CommandArg(int, "Characters to delete"))
@delback_cmd.hook(500)
def delback_cb(n):
    cursors.current.delete(1)

@curmap.add(Key("BACKSPACE"))
def delback_mapped(keys):
    delback_cmd.run_withargs(1)

# cursor-activate
#
# Activates the current cursor. Exactly what the hell
# this does depends on the specific cursor. Called by
# control + a.

activate_cmd = Command("cursor-activate")
@activate_cmd.hook(500)
def activate_cb():
    cursors.current.activate()

@curmap.add(Key("A", con=True))
def activate_mapped(keys):
    editor.buffers.userlog.log("Activated cursor")
    activate_cmd.run()

# cursor-activate
#
# Activates the current cursor. Exactly what the hell
# this does depends on the specific cursor. Called by
# escape (or alt) + a.

deactivate_cmd = Command("cursor-deactivate")
@deactivate_cmd.hook(500)
def deactivate_cb():
    cursors.current.deactivate()

@curmap.add(Key("D", con=True))
def deactivate_mapped(keys):
    editor.buffers.userlog.log("Deactivated cursor")
    deactivate_cmd.run()

# cursor-enter
#
# Inserts a line-break at the position of the cursor.
# Called by RETURN.

enter_cmd = Command("cursor-enter")
@enter_cmd.hook(500)
def enter_cb():
    cursors.current.enter()

@curmap.add(Key("RETURN"))
def enter_mapped(keys):
    enter_cmd.run()

# cursor-insert
#
# Inserts a string of characters into a buffer. Called
# by selected printable characters.

insertable_chars = ("!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~ "
                    "abcdefghijklmnopqrstuvwxyz"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "0123456789")

insert_cmd = Command("cursor-insert",
                     CommandArg(str))
@insert_cmd.hook(500)
def insert_cb(string):
    cursors.current.insert(string)

def insert_mapped(keys):
    keystring = str(keys[-1])

    insert_cmd.run_withargs(keystring)

for char in insertable_chars:
    curmap.add(Key(char))(insert_mapped)

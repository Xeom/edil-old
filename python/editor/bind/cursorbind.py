import string

import core.keymap
import editor.buffers.userlog

from core.key import Key
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
    sel = core.cursor.get_selected()
    sel.move_lines(-n)

up_cmd.map_to(curmap, Key("UP"), defaultargs=[1])

# cursor-down
#
# Moves the cursor down a specified number of lines.
# Negative values move the cursor down. Called with
# argument 1 by DOWN key.

down_cmd = Command("cursor-down",
                   CommandArg(int, "Lines to move down"))
@down_cmd.hook(500)
def down_cb(n):
    sel = core.cursor.get_selected()
    sel.move_lines(n)

down_cmd.map_to(curmap, Key("DOWN"), defaultargs=[1])

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
    sel = core.cursor.get_selected()
    sel.move_cols(-n)

back_cmd.map_to(curmap, Key("LEFT"), defaultargs=[1])

# cursor-forward
#
# Moves the cursor forward in its buffer a specified
# number of characters. If the cursor reaches the
# end of a line, it goes to the end line. Called
# with argument 1 by the RIGHT key.

forward_cmd = Command("cursor-forward",
                      CommandArg(int, "Characters to move forward"))
@forward_cmd.hook(500)
def forward_cb(n):
    sel = core.cursor.get_selected()
    sel.move_cols(n)

forward_cmd.map_to(curmap, Key("RIGHT"), defaultargs=[1])

# cursor-delback
#
# Deletes a specified number of characters behind the
# cursor. Called with argument 1 by the BACK key.

delback_cmd = Command("cursor-delback",
                      CommandArg(int, "Characters to delete"))
@delback_cmd.hook(500)
def delback_cb(n):
    sel = core.cursor.get_selected()
    sel.delete(n)

delback_cmd.map_to(curmap, Key("BACKSPACE"), defaultargs=[1])

# cursor-activate
#
# Activates the current cursor. Exactly what the hell
# this does depends on the specific cursor. Called by
# control + a.

activate_cmd = Command("cursor-activate")
@activate_cmd.hook(500)
def activate_cb():
    editor.buffers.userlog.log("Activated cursor")
    sel = core.cursor.get_selected()
    sel.activate()

activate_cmd.map_to(curmap, Key("A", con=True))

# cursor-activate
#
# Activates the current cursor. Exactly what the hell
# this does depends on the specific cursor. Called by
# escape (or alt) + a.

deactivate_cmd = Command("cursor-deactivate")
@deactivate_cmd.hook(500)
def deactivate_cb():
    editor.buffers.userlog.log("Deactivated cursor")
    sel = core.cursor.get_selected()
    sel.deactivate()

deactivate_cmd.map_to(curmap, Key("a", esc=True))

# cursor-enter
#
# Inserts a line-break at the position of the cursor.
# Called by RETURN.

enter_cmd = Command("cursor-enter")
@enter_cmd.hook(500)
def enter_cb():
    sel = core.cursor.get_selected()
    sel.enter()

enter_cmd.map_to(curmap, Key("RETURN"))

# cursor-insert
#
# Inserts a string of characters into a buffer. Called
# by selected printable characters.

insertable_chars = ("!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~ "
                    "abcdefghijklmnopqrstuvwxyz"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "0123456789")

insert_cmd = Command("cursor-insert",
                     CommandArg(str, "String to insert"),
                     CommandArg(int, "Repetitions"))
@insert_cmd.hook(500)
def insert_cb(string, n):
    if isinstance(string, str):
        string = string.encode("ascii")

    string *= n

    sel = core.cursor.get_selected()
    sel.insert(string)

for char in insertable_chars:
    insert_cmd.map_to(curmap, Key(char), defaultargs=[char, 1])


# cursor-insert-hex
#
# Insert a character, takes argument in hex, bin, oct etc.

insert_hex_cmd = Command("cursor-insert-hex",
                         CommandArg(str,
                                    "Character (0x.., 0b.., 0o.., 1..)"))
@insert_hex_cmd.hook(500)
def insert_hex_cb(number):
    char = chr(int(number, base=0))

    if char == "\n":
        enter_cmd.run()

    else:
        insert_cmd.run_withargs(char)

# cursor-goto-line
#
# Move the cursor to a specific line number

goto_line_cmd = Command("cursor-goto-line",
                        CommandArg(int, "Lineno to go to"))

@goto_line_cmd.hook(500)
def goto_line_cb(ln):
    sel = core.cursor.get_selected()
    sel.ln = ln

# cursor-goto-col
#
# Move the cursor to a specific column number
# if the number is negative, move it that many spaces back from the end

goto_col_cmd = Command("cursor-goto-col",
                       CommandArg(int, "Colno to go to"))

@goto_col_cmd.hook(500)
def goto_col_cb(cn):
    sel = core.cursor.get_selected()

    line = sel.buffer[sel.ln]

    if line:
        cn %= len(line) + 1
    else:
        cn = 0

    sel.cn = cn

goto_col_cmd.map_to(curmap, Key("HOME"), defaultargs=[0])
goto_col_cmd.map_to(curmap, Key("END"), defaultargs=[-1])

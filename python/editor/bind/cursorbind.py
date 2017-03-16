import string

import editor.buffers.userlog
import core.cursor

from core.mode      import Mode
from core.key       import Key
from editor.command import Command, CommandArg

mode = Mode.new(100, "default-cursor")
kmap = mode.keymap

mapname = "cursor-default"

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

up_cmd.map_to(kmap, Key("UP"), defaultargs=[1])

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

down_cmd.map_to(kmap, Key("DOWN"), defaultargs=[1])

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

back_cmd.map_to(kmap, Key("LEFT"), defaultargs=[1])

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

forward_cmd.map_to(kmap, Key("RIGHT"), defaultargs=[1])

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

delback_cmd.map_to(kmap, Key("BACKSPACE"), defaultargs=[1])

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

activate_cmd.map_to(kmap, Key("A", con=True))

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

deactivate_cmd.map_to(kmap, Key("a", esc=True))

# cursor-enter
#
# Inserts a line-break at the position of the cursor.
# Called by RETURN.

enter_cmd = Command("cursor-enter")
@enter_cmd.hook(500)
def enter_cb():
    sel = core.cursor.get_selected()
    sel.enter()

enter_cmd.map_to(kmap, Key("RETURN"))

# cursor-insert
#
# Inserts a string of characters into a buffer. Called
# by selected printable characters.

insertable_chars = ("!\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~ "
                    "abcdefghijklmnopqrstuvwxyz"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "0123456789")

insert_cmd = Command("cursor-insert",
                     CommandArg(int, "Repetitions")
                     CommandArg(str, "String to insert"))
@insert_cmd.hook(500)
def insert_cb(n, string):
    string = string.encode("utf-8")
    string *= n

    sel = core.cursor.get_selected()
    sel.insert(string)

# Bind cursor-insert to every printable ascii character
for char in insertable_chars:
    insert_cmd.map_to(kmap, Key(char), defaultargs=[1, char])

# A class to help interpret wide characters when they are inserted
class Utf8_Processor:
    def __init__(self):
        self.codepoint = 0
        self.width     = 0
        self.currlen   = 0
        self.repeats   = 1

    def repeat(self, n):
        self.repeats = max(self.repeats, n)

    def process_continue(self, keycode):
        self.codepoint <<= 6
        self.codepoint  |= keycode & 0x3f
        self.currlen    += 1

        if self.currlen == self.width:
            rtn = chr(self.codepoint) * self.repeats
            self.repeats   = 1
            return rtn

    def process_start(self, keycode):
        if   keycode < 0xe0: self.width = 2
        elif keycode < 0xf0: self.width = 3
        elif keycode < 0xf8: self.width = 4

        self.currlen   = 1
        self.codepoint = keycode & (0x7f >> self.width)

    def process_ascii(self, keycode):
        rtn = chr(keycode) * self.repeats
        self.repeats = 1

        return rtn

    def process(self, keycode):
        if   keycode < 0x80:
            return self.process_ascii(keycode)

        elif keycode < 0xc0:
            return self.process_continue(keycode)

        else:
            return self.process_start(keycode)

utf8_processor  = Utf8_Processor()

# cursor-insert-utf8
#
# Insert a utf-8 encoded byte
#
# When the command is called multiple times until a full utf-8 character is
# completed, the character is inserted.

insert_utf8_cmd = Command("cursor-insert-utf8",
                          CommandArg(int, "Next UTF-8 character code"),
                          CommandArg(int, "Repetitions"))

@insert_utf8_cmd.hook(500)
def insert_utf8_cb(n, keycode):
    if keycode > 0xf9:
        return

    utf8_processor.repeat(n)
    string = utf8_processor.process(keycode)

    if string:
        insert_cmd.run_withargs(string, 1)

# Bind cursor-insert-utf8 to all the utf-8 specific keycodes
for char in range(0x80, 0xf9):
    insert_utf8_cmd.map_to(kmap,
                           Key("<{:04x}>".format(char)), defaultargs=[1, char])

# cursor-insert-codepoint
#
# Insert a character, with a specific codepoint

insert_hex_cmd = Command("cursor-insert-codepoint",
                         CommandArg(lambda s:int(s, base=0),
                                    "Character (0x.., 0b.., 0o.., 1..)"))
@insert_hex_cmd.hook(500)
def insert_hex_cb(codepoint):
    char = chr(codepoint)

    if char == "\n":
        enter_cmd.run()

    else:
        insert_cmd.run_withargs(char, 1)

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

goto_col_cmd.map_to(kmap, Key("HOME"), defaultargs=[0])
goto_col_cmd.map_to(kmap, Key("END"), defaultargs=[-1])

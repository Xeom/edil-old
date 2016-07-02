import core.keymap
import core.cursor
import core.deferline

from core.key import Key
from core.face import Face
from editor.command import Command, CommandArg

mapname = "tabs-default"

core.keymap.maps.add(mapname)
tabmap = core.keymap.maps[mapname]

default_tab_string = b" " + Face(Face.black, Face.cyan).colour(b"...")
default_indent_string = b"\t"

@core.deferline.hooks.draw(500)
def expandtabs(w, b, ln, li):
    tab = get_tab_string(b)

    for i, c in enumerate(li.vec):
        if c.value == b'\t':
            li.delete(i)
            li.insert(i, tab)

def get_indent_string(buf):
    prop = buf.properties["indent-string"]

    if prop == None:
        buf.properties["indent-string"] = default_indent_string

        return default_indent_string

    return prop

def set_indent_string(buf, string):
    buf.properties["indent-string"] = string

def get_tab_string(buf):
    prop = buf.properties["tab-string"]

    if prop == None:
        buf.properties["tab-string"] = default_tab_string

        return default_tab_string

    return prop

def set_tab_string(buf, string):
    buf.properties["tab-string"] = string

def get_indent_of_line(buf, ln):
    indent = get_indent_string(buf)
    text   = bytes(buf[ln])
    blocks = (text[n:n+len(indent)] for n in range(0, len(text), len(indent)))

    n = 0

    for block in blocks:
        if block == indent:
            n += 1

        else:
            break

    return n

def set_indent_of_line(buf, ln, n, cur):
    indent = get_indent_string(buf)
    text   = bytes(buf[ln])

    chars = 0
    blocks = (text[n:n+len(indent)] \
              for n in range(0, len(text), len(indent)))

    for block in blocks:
        if block == indent:
            chars += len(indent)

        else:
            for c in indent:
                if c == text[chars]:
                    chars += 1

                else:break
            break

    tomove = len(indent) * n - chars

    if tomove < 0:
        cur.move_cols(max(tomove, -cur.cn))
    buf[ln] = (indent * n) + text[chars:]
    if tomove >= 0:
        cur.move_cols(tomove)

insert_cmd = Command("tab-insert")

indent_cmd = Command("tab-indent",
                     CommandArg(int, "Levels to change"))

@indent_cmd.hook(500)
def indent_cb(n):
    cur = core.cursor.get_selected()
    lvl = get_indent_of_line(cur.buffer, cur.ln) + n

    if lvl < 0:
        return

    set_indent_of_line(cur.buffer, cur.ln, lvl, cur)

@tabmap.add(Key("TAB"), Key("RIGHT"))
def indent_right_mapped(keys):
    indent_cmd.run_withargs(1)

@tabmap.add(Key("TAB"), Key("LEFT"))
def indent_left_mapped(keys):
    indent_cmd.run_withargs(-1)

align_cmd = Command("tab-align",
                    CommandArg(int, "Relative lineno to copy alignment from"))

@align_cmd.hook(500)
def align_cb(n):
    cur = core.cursor.get_selected()
    ln  = cur.ln + n

    if ln < 0:
        return

    if ln > len(cur.buffer):
        return

    lvl = get_indent_of_line(cur.buffer, ln)
    set_indent_of_line(cur.buffer, cur.ln, lvl, cur)

@tabmap.add(Key("TAB"), Key("DOWN"))
def align_up_mapped(keys):
    align_cmd.run_withargs(1)

@tabmap.add(Key("TAB"), Key("UP"))
def align_down_mapped(keys):
    align_cmd.run_withargs(-1)

import itertools

import core.cursor
import core.deferline
import core.ui

from core.mode import Mode
from core.key  import Key
from core.face import Face
from editor.command import Command, CommandArg

mapname = "tabs-default"

mode = Mode(100, "default-tabs")
kmap = mode.keymap

default_tab_string    = b"   " + Face(Face.black, Face.cyan).colour(b">")
default_indent_string = b"\t"
default_tab_align     = True

"""Hooked function to display tabs properly.

This function expands tab characters in each line that's rendered into
default_tab_string. This is generally to make them more visible.
"""
@core.deferline.hooks.draw(600)
def expandtabs(w, b, ln, li):
    tab    = get_tab_string(b)
    align  = get_tab_align(b)

    if align:
        tablen = 0
        subtab = core.ui.text_next_char(tab)
        while subtab:
            subtab  = core.ui.text_next_char(subtab[1:])
            tablen += 1

        colnum = 0
        for i, c in enumerate(li.vec):
            if c.value == b'\t':
                li.delete(i)

                skiplen = colnum % tablen
                subtab  = tab
                import sys
                print(skiplen, file=sys.stderr)
                for n in range(skiplen):
                    subtab = core.ui.text_next_char(subtab[1:])

                li.insert(i, subtab)
                colnum += tablen - skiplen

            else:
                colnum += 1

    else:
        exit()
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

def get_tab_align(buf):
    prop = buf.properties["tab-align"]

    if prop == None:
        buf.properties["tab-align"] = str(default_tab_align)

        return default_tab_align

    return prop == b"True"

def set_tab_align(buf, v):
    buf.properties["tab-align"] = "True" if v else "False"

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

def get_last_nonzero_indent(buf, ln):
    minbound = max(0, ln - 100)
    while ln > minbound:

        ln -= 1
        indent = get_indent_of_line(buf, ln)

        if indent != 0:
            return indent

    return 0

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
    import sys
    print((indent * n) + text[chars:], file=sys.stderr)
    buf[ln] = (indent * n) + text[chars:]

    if tomove >= 0:
        cur.move_cols(tomove)

indent_cmd = Command("tab-indent",
                           CommandArg(int, "Levels to change"))

@indent_cmd.hook(500)
def indent_cb(n):
    cur = core.cursor.get_selected()
    lvl = get_indent_of_line(cur.buffer, cur.ln) + n

    if lvl < 0:
        return

    set_indent_of_line(cur.buffer, cur.ln, lvl, cur)


indent_cmd.map_to(kmap, Key("TAB"), Key("RIGHT"), defaultargs=[ 1])
indent_cmd.map_to(kmap, Key("TAB"), Key("LEFT"),  defaultargs=[-1])

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

align_cmd.map_to(kmap, Key("TAB"), Key("DOWN"), defaultargs=[ 1])
align_cmd.map_to(kmap, Key("TAB"), Key("UP"),   defaultargs=[-1])

align_last_nonzero_cmd = Command("tab-align-last-nonzero")

@align_last_nonzero_cmd.hook(500)
def align_last_nonzero_cb():
    cur = core.cursor.get_selected()

    lvl = get_last_nonzero_indent(cur.buffer, cur.ln)
    set_indent_of_line(cur.buffer, cur.ln, lvl, cur)

align_cmd.map_to(kmap, Key("TAB"), Key("TAB"), defaultargs=[])

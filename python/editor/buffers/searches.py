import re

import core.ui
import core.cursor

from core.buffer import Buffer
from core.cursor import Cursor
from core.face   import Face

buffer = Buffer.new()
cursor = core.cursor.spawn(buffer, core.cursor.types.point)
stream = cursor.open_stream()

linkface = Face(Face.black, Face.green)

stream.write(b"""
--------------------------------------------------

This is the search buffer.

Here, the results of searches you make are
displayed. This buffer will automatically appear
when you make a search for something.

--------------------------------------------------
""")
stream.flush()

@core.deferline.hooks.draw(500)
def colour_links(w, b, ln, li):
    if b != buffer:
        return

    def sub(m):
        start = m.start()
        end   = m.end()
        li.insert(start, linkface.serialize(end - start))

    re.sub(b"\\b//.+:\\d+//", sub, bytes(li.vec))


class Result:
    @classmethod
    def from_cursor(cur):
        for match in re.finditer(b"\\b//.+:\\d+//", CUNT):
            if cur.cn in range(m.start(), m.end()):
                return Result(m.group(0))

    @classmethod
    def from_fn_ln(fn, ln):
        return Result("//" + fn + ":" + str(ln) + "//")

    def __init__(self, string):
        self.string = string

    def __str__(self):
        return "//" + self.string + "//"

    def goto(self):
        for buf in editor.buffers.ring.buffers:
            buf.properties["filename"]

def results(desc, results):
    root = core.windows.get_root()

    if not any(buffer == s.buffer for s in root):
        new = root.split(core.windows.direction.right)
        new.buffer = buffer


def log(string):
    root = core.windows.get_root()

    if not any(buffer == s.buffer for s in root):
        new = root.split(core.windows.direction.right)
        new.buffer = buffer

    if isinstance(string, str):
        string = string.encode("ascii")

    stream.write(string + b"\n")
    stream.flush()

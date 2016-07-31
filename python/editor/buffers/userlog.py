import time

import core.ui
import core.cursor

from core.buffer import Buffer
from core.cursor import Cursor
from core.face   import Face

buffer = Buffer.new()
cursor = core.cursor.spawn(buffer, core.cursor.types.point)
stream = cursor.open_stream()

face = Face(Face.black, Face.green)

stream.write(b"""
--------------------------------------------------

This is the userlog buffer.

Here, user information events that appear in the
statusbar are recorded. This is useful for viewing
statusbar changes that happen too quickly to be
seen normally.

--------------------------------------------------
""")
stream.flush()

def log(string):
    if isinstance(string, str):
        string = string.encode("ascii")

    core.ui.set_sbar(face.serialize(len(string)) + string)
    stream.write(time.strftime("[%H:%M:%S] ").encode("ascii") + string + b"\n")
    stream.flush()

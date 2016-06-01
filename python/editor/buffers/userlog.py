import core.ui

from editor.cursor.cursor import cursors
from core.buffer import Buffer
from core.point  import Point
from core.face   import Face

buffer = Buffer.new()
point  = cursors.get_buffer_cursors(buffer).spawn(cls=Point)
stream = point.open_stream()

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
    stream.write(string + b"\n")
    stream.flush()

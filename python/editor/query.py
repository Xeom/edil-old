from core.key import Key
from core.face import Face
import core.keymap
import core.ui
import editor.cursor.cursor as cursor
import editor.cursor

query_prefix_face = Face(Face.black, Face.cyan)

class QueryPoint:
    def __init__(self, buffer):
        self.ln       = 0
        self.cn       = 0
        self.string   = []
        self.buffer   = buffer
        self.callback = None
        self.prefix   = b""

        self.draw()

    @property
    def prefix(self):
        raise AttributeError

    @prefix.setter
    def prefix(self, v):
        if isinstance(v, str):
            v = v.encode("ascii")

        if v:
            self.pre = query_prefix_face.serialize(len(v)) + v
        else:
            self.pre = b""

        self.draw()

    def draw(self):
        string = bytearray("".join(self.string), "ascii")
        string[self.cn:self.cn] = cursor.cursor_face.serialize(1)

        core.ui.set_sbar(self.pre + bytes(string))

    def move_cols(self, n):
        self.cn += n
        self.cn  = min(len(self.string), self.cn)
        self.cn  = max(0,                self.cn)

        self.draw()

    def move_lines(self, n):
        pass

    def delete(self, n):
        del self.string[max(0, self.cn - n):self.cn]
        self.cn -= n
        self.cn  = max(0, self.cn)

        self.draw()

    def insert(self, string):
        if isinstance(string, bytes):
            string = string.decode("ascii")

        self.string[self.cn:self.cn] = list(string)
        self.cn += len(string)

        self.draw()

    def enter(self):
        pass

    def __lt__(self, other):
        return self.cn < other.cn

    def __le__(self, other):
        return self.cn <= other.cn

    def __gt__(self, other):
        return self.cn > other.cn

    def __ge__(self, other):
        return self.cn >= other.cn

    def __eq__(self, other):
        return self.cn == other.cn

    def __sub__(self, other):
        return self.cn - other.cn

mapname = "query-default"

core.keymap.maps.add(mapname)
querymap = core.keymap.maps[mapname]

querying_buffers = set()

@querymap.add(Key("J", con=True))
def query_enter(keys):
    leave_query()

def make_query(callback, prefix=b""):
    core.keymap.frames.active.push("query-default")

    curs = cursor.cursors.current_buffer_cursors
    buf  = core.windows.get_selected().buffer

    if buf in querying_buffers:
        raise Exception("Cannot double-query")

    querying_buffers.add(buf)

    curs.spawn(cls=QueryPoint, select=True)
    curs.selected.callback = callback
    curs.selected.prefix   = prefix

def leave_query():
    cur = cursor.cursors.current
    buf = core.windows.get_selected().buffer

    core.keymap.frames.active.remove("query-default")

    querying_buffers.remove(buf)

    if not isinstance(cur, QueryPoint):
        raise Exception("Not in a query")

    cursor.cursors.current_buffer_cursors.remove_selected()
    core.ui.set_sbar(b"")
    cur.callback("".join(cur.string))
    

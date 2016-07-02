from core.key import Key
from core.face import Face
import core.keymap
import core.ui

from core.cursor import CursorType
import core.cursor

import os

query_prefix_face = Face(Face.black, Face.cyan)

class AutoCompleter:
    def __init__(self, callback):
        self.options = []
        self.index   = 0
        self.last    = None
        self.cb      = callback

    def nextoption(self):
        if not self.options:
            return None

        rtn = self.options[self.index]

        self.index += 1
        self.index %= len(self.options)

        return rtn

    def complete(self, semi):
        if self.cb == None:
            return None

        if self.last != semi:
            self.options = self.cb(semi)
            self.index   = 0

        rtn = self.nextoption()

        self.last = rtn

        return rtn

class QueryCursor:
    def __init__(self):
        self.cn = 0
        self.string = []
        self.prefix = core.ui.get_sbar()
        self.draw()

    def init(self, b):
        self.buffer = b

    def get_buffer(self):
        return self.buffer

    def get_ln(self):
        return 0

    def get_cn(self):
        return self.cn

    def draw(self):
        string = bytearray("".join(self.string), "ascii")
        string[self.cn:self.cn] = core.ui.cursorface

        core.ui.set_sbar(self.prefix + bytes(string))

    def move_cols(self, n):
        self.cn += n
        self.cn  = min(len(self.string), self.cn)
        self.cn  = max(0,                self.cn)

        self.draw()

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

    def set(self, string):
        if isinstance(string, bytes):
            string = string.decode("ascii")

        self.string = list(string)
        self.cn     = len(string)

        self.draw()

QueryCursorType = CursorType(QueryCursor)

#cursor.Snapper.blacklist.append(QueryPoint)

mapname = "query-default"

core.keymap.maps.add(mapname)
querymap = core.keymap.maps[mapname]

querying_buffers = set()

def autocomplete_query():
    curcursor = core.cursor.get_selected()
    curinst   = QueryCursorType.find_instance(curcursor)

    if not isinstance(curinst, QueryCursor):
        raise Exception("Not in a query")

    auto = curinst.autocompleter.complete("".join(curinst.string))

    if auto != None:
        curinst.set(auto)

def make_query(callback, prefix=b"", completecallback=None):
    core.keymap.frames.active.push("query-default")

    buf  = core.windows.get_selected().buffer

    if buf in querying_buffers:
        raise Exception("Cannot double-query")

    if isinstance(prefix, str):
        prefix = prefix.encode("ascii")

    core.ui.set_sbar(query_prefix_face.serialize(len(prefix)) + prefix)

    querying_buffers.add(buf)

    newcursor = core.cursor.spawn(buf, QueryCursorType)
    core.cursor.select_last(buf)
    newinst   = QueryCursorType.find_instance(newcursor)
    newinst.autocompleter = AutoCompleter(completecallback)
    newinst.callback      = callback

def leave_query():
    curcursor = core.cursor.get_selected()
    curinst   = QueryCursorType.find_instance(curcursor)

    buf = core.windows.get_selected().buffer

    core.keymap.frames.active.remove("query-default")

    if buf in querying_buffers:
        querying_buffers.remove(buf)

    core.ui.set_sbar(b"")
    core.cursor.delete_selected(buf)
    curinst.callback("".join(curinst.string))

@querymap.add(Key("RETURN"))
def query_enter(keys):
    leave_query()

@querymap.add(Key("TAB"))
def query_tab(keys):
    autocomplete_query()


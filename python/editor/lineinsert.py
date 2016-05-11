import core.ui

from core.hook import NativeHook

class LineInserter:
    def __init__(self, vec):
        self.vec     = vec
        self.inserts = {}

    def insert(self, index, string):
        if isinstance(string, str):
            string = string.encode("ascii")

        if index in self.inserts:
            self.inserts[index] += string

        else:
            self.inserts[index] = string

    def dump(self):
        items = sorted(self.inserts.items(), key=lambda x:x[0], reverse=True)

        for index, insert in items:
            self.vec.insert_bytes(index, insert)

        self.inserts = {}

class hooks:
    draw_line = NativeHook()

@core.ui.hooks.win.content.draw_line_pre(150)
def handle_draw_line(w, b, ln, v):
    inserter = LineInserter(v)

    hooks.draw_line.call([w, b, ln, v, inserter])

    inserter.dump()

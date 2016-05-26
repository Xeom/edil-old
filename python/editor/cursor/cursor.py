import symbols.lib

import core.windows
import core.ui

from core.face import Face

class BufferCursors:
    default = None

    def __init__(self, buffer):
        self.cursors   = []
        self.selectind = 0

        self.buffer   = buffer

    def spawn(self, cls=None, select=False):
        if cls == None:
            cls = self.default

        self.cursors.append(cls(self.buffer))

        if select:
            self.selectind = len(self.cursors) - 1

    def remove_selected(self):
        del self.cursors[self.selectind]

        if self.selectind >= len(self.cursors):
            self.selectind = len(self.cursors) - 1

    def select_next(self):
        self.selectind += 1
        self.selectind %= len(self.cursors)

    def select_prev(self):
        self.selectind -= 1
        self.selectind %= len(self.cursors)

    @property
    def selected(self):
        if not self.cursors:
            return None

        return self.cursors[self.selectind]

class Cursors:
    default = None
    def __init__(self):
        self.buffers  = {}

    def get_buffer_cursor(self, buffer):
        if buffer not in self.buffers:
            new = BufferCursors(buffer)
            self.buffers[buffer] = new

            new.spawn()

        return self.buffers[buffer].selected

    @property
    def current_buffer_cursors(self):
        b = core.windows.get_selected().buffer

        if b not in self.buffers:
            new = BufferCursors(b)
            self.buffers[b] = new

            new.spawn()

        return self.buffers[b]

    @property
    def current(self):
        buffer = core.windows.get_selected().buffer

        return self.get_buffer_cursor(buffer)

cursors = Cursors()
cursor_face = Face(Face.white, Face.black)

@core.deferline.hooks.draw(300)
def handle_line_draw(w, b, ln, li):
    if w != core.windows.get_selected():
        return

    if cursors.current == None:
        return

    if ln.value != cursors.current.ln:
        return

    li.insert(cursors.current.cn, cursor_face.serialize(1))

@core.windows.hooks.select(800)
def handle_win_select(w1, w2):
    core.ui.draw_window_line(w1, cursors.get_buffer_cursor(w1.buffer).ln)
    core.ui.draw_window_line(w2, cursors.get_buffer_cursor(w2.buffer).ln)

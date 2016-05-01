import symbols.lib

import core.windows
import editor.cursor.point

class BufferCursors:
    default = None

    def __init__(self, buffer):
        self.cursors   = []
        self.selectind = 0

        self.buffer   = buffer

    def spawn(self, cls=None):
        if cls == None:
            cls = self.default
        self.cursors.append(cls(self.buffer))

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
        self.buffers = {}

    def get_buffer_cursor(self, buffer):
        if buffer not in self.buffers:
            new = BufferCursors(buffer)
            self.buffers[buffer] = new

            new.spawn()

        return self.buffers[buffer].selected

    @property
    def current(self):
        buffer = core.windows.get_selected().buffer

        return self.get_buffer_cursor(buffer)

cursors = Cursors()

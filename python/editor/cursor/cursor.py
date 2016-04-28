import core.windows

from editor.cursor.regioncursor import RegionCursor

class BufferCursors:
    def __init__(self, buffer):
        self.cursors   = []
        self.selectind = 0

        self.buffer   = buffer

    def spawn(self, cls=RegionCursor):
        self.cursors.append(cls(self.buffer))

    def select_next(self):
        self.selectind += 1
        self.selectind %= len(self.cursors)

    def select_prev(self):
        self.selectind -= 1
        self.selectind %= len(self.cursors)

    @property
    def selected(self):
        return self.cursors[self.selectind]

class Cursors:
    def __init__(self):
        self.buffers = {}

    def get_buffer_cursors(self, buffer):
        if buffer not in self.buffers:
            new = BufferCursors(buffer)
            self.buffers[buffer] = new

            new.spawn()

        return self.buffers[buffer].selected

    @property
    def current(self):
        buffer = core.windows.get_selected().buffer

        return self.get_buffer_cursors(buffer)

cursors = Cursors()

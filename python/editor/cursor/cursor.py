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

        new = cls(self.buffer)
        self.cursors.append(new)

        if select:
            self.selectind = len(self.cursors) - 1

        return new

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

    def get_buffer_cursors(self, buffer):
        if buffer not in self.buffers:
            new = BufferCursors(buffer)
            self.buffers[buffer] = new

            new.spawn()

        return self.buffers[buffer]

    @property
    def current_buffer_cursors(self):
        b = core.windows.get_selected().buffer

        return self.get_buffer_cursors(b)

    @property
    def current(self):
        buffer = core.windows.get_selected().buffer

        return self.get_buffer_cursor(buffer)

class Snapper:
    modes = ["centre",
             "top",
             "bottom",
             "minimal"]
    blacklist = []

    def __init__(self):
        self.mode = "minimal"

    def next_mode(self):
        currind = self.modes.index(self.mode)
        self.set_mode(self.modes[(currind + 1) % len(self.modes)])

    def set_mode(self, mode):
        if mode not in self.modes:
            raise Exception("Invalid snap mode")

        self.mode = mode
        self.snap()

    def snap(self):
        cur = cursors.current
        win = core.windows.get_selected()

        if any(isinstance(cur, i) for i in self.blacklist):
            return

        if cur.buffer != win.buffer:
            return

        {
            "centre" : self.snap_centre,
            "top"    : self.snap_top,
            "bottom" : self.snap_bottom,
            "minimal": self.snap_minimal
        }[self.mode](win, cur)


class YSnapper(Snapper):
    def snap_centre(self, win, cur):
        win.offsety = max(0, cur.ln - win.textsize[1] // 2)

    def snap_top(self, win, cur):
        win.offsety = cur.ln

    def snap_bottom(self, win, cur):
        win.offsety = max(0, cur.ln - win.textsize[1] + 1)

    def snap_minimal(self, win, cur):
        if cur.ln >= win.textsize[1] + win.offsety:
            self.snap_bottom(win, cur)

        elif cur.ln < win.offsety:
            self.snap_top(win, cur)

class XSnapper(Snapper):
    def snap_centre(self, win, cur):
        xpos = core.ui.get_window_cursor_posx(win, cur)
        win.offsetx = max(0, xpos - win.textsize[0] // 2)

    def snap_top(self, win, cur):
        xpos = core.ui.get_window_cursor_posx(win, cur)
        win.offsetx = xpos

    def snap_bottom(self, win, cur):
        xpos = core.ui.get_window_cursor_posx(win, cur)
        win.offsetx = max(0, xpos - win.textsize[0] + 1)

    def snap_minimal(self, win, cur):
        xpos = core.ui.get_window_cursor_posx(win, cur)
        if xpos >= win.textsize[0] + win.offsetx:
            self.snap_bottom(win, cur)

        elif xpos < win.offsetx:
            self.snap_top(win, cur)

cursors  = Cursors()
xsnapper = XSnapper()
ysnapper = YSnapper()

@core.deferline.hooks.draw(300)
def handle_line_draw(w, b, ln, li):
    if w != core.windows.get_selected():
        return

    if cursors.current == None:
        return

    if ln.value != cursors.current.ln:
        return

    li.insert(cursors.current.cn, core.face.cursor_serial)

@core.windows.hooks.select(800)
def handle_win_select(w1, w2):
    core.ui.draw_window_line(w1, cursors.get_buffer_cursor(w1.buffer).ln)
    core.ui.draw_window_line(w2, cursors.get_buffer_cursor(w2.buffer).ln)

@core.point.hooks.move_post(800)
def handle_point_move(point, oldln, oldcn):
    win = core.windows.get_selected()
    cur = cursors.current

    if len(win.buffer) == 0:
        return

    if cur.ln >= win.textsize[1] + win.offsety or \
       cur.ln < win.offsety:
        ysnapper.snap()

    xpos = core.ui.get_window_cursor_posx(win, cur)

    if win.offsetx and xpos < win.textsize[0]:
        win.offsetx = 0
    elif xpos >= win.textsize[0] + win.offsetx or \
       xpos < win.offsetx:
        xsnapper.snap()
    

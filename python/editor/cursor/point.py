import core.buffer
import core.ui

from symbols.vec import str2vec
from core.hook   import NativeHook

class Point:
    def __init__(self, buffer):
        self._ln = None
        self._cn = None

        self.buffer = buffer

        self.cn = 0
        self.ln = 0

        @core.buffer.hooks.line.delete_post(500)
        def handle_line_delete(b, ln):
            if self.ln >= ln.value and self.buffer == b:
                self.ln -= 1

                self.correct_linepos()
                self.correct_colpos()

        self.handle_line_delete = handle_line_delete

        @core.buffer.hooks.line.change_post(500)
        def handle_line_change(b, ln, v):
            if self.ln == ln.value and self.buffer == b:
                self.correct_colpos()

        self.handle_line_change = handle_line_change

        @core.buffer.hooks.line.insert_post(500)
        def handle_line_insert(b, ln):
            if self.ln >= ln.value and self.buffer == b:
                self.ln += 1

                self.correct_linepos()
                self.correct_colpos()

        self.handle_line_insert = handle_line_insert

    def __lt__(self, other):
        return self.ln < other.ln \
            or (self.ln == other.ln and self.cn < other.cn)

    def __gt__(self, other):
        return other.__lt__(self)

    def __eq__(self, other):
        return isinstance(other, Point) and \
            self.ln == other.ln \
            and self.cn == other.cn

    def __le__(self, other):
        return self.__lt__(other) \
            or self.__eq__(other)

    def __ge__(self, other):
        return self.__gt__(other) \
            or self.__eq__(other)

    def move_cols(self, n):
        bufferlen = len(self.buffer)

        while n > len(self.line) - self.cn:
            n -= len(self.line)
            n += self.cn
            self.cn  = 0
            self.ln += 1

            if self.ln >= bufferlen:
                self.ln -= 1
                break

        self.cn += n

        self.correct_colpos()

    def move_lines(self, n):
        self.ln += n

        self.correct_linepos()
        self.correct_colpos()

    def merge_line_back(self):
        if self.ln == 0:
            return

        oldline = self.line

        self.ln  -= 1

        self.buffer.delete(self.ln + 1)

        newline   = self.line
        self.cn = len(newline)
        newline  += oldline
        self.line = newline

        return

    def delete_char(self):
        if not len(self.buffer):
            return

        if self.cn == 0:
            self.merge_line_back()
            return

        self.cn -= 1
        l = self.line
        del l[self.cn]
        self.line = l

    def insert(self, string):
        l = self.line

        if isinstance(string, str):
            string = string.encode("ascii")

        l.insert_bytes(self.cn, string)

        self.cn += len(string)

        self.line = l

    def insert_char(self, char):
        l = self.line
        l.insert(self.cn, char)
        self.line = l

        self.cn += 1

    def enter(self):
        start = self.line
        start.delete(self.cn, len(start) - self.cn)

        end   = self.line
        end.delete(0, self.cn)

        self.line = start
        self.cn   = 0

        self.buffer.insert(self.ln + 1)

        self.ln += 1

        self.line = end

    def correct_colpos(self):
        if not len(self.buffer):
            self.cn = 0
            return

        linelen = len(self.line)

        if self.cn < 0:
            self.cn = 0

        if self.cn >= linelen:
            self.cn = linelen

    def correct_linepos(self):
        length = len(self.buffer)

        if length == 0:
            self.ln = 0

        elif self.ln >= length:
            self.ln = length - 1

    @property
    def cn(self):
        return self._cn

    @cn.setter
    def cn(self, value):
        old = self._cn
        hooks.move_pre.call([self])
        self._cn = value
        hooks.move_post.call([self])
        hooks.move_cn.call([self, old, value])

    @property
    def ln(self):
        return self._ln

    @ln.setter
    def ln(self, value):
        old = self._ln
        hooks.move_pre.call([self])
        self._ln = value
        hooks.move_post.call([self])
        hooks.move_ln.call([self, old, value])

    @property
    def line(self):
        if len(self.buffer) == 0:
            self.buffer.insert(0)

        return self.buffer[self.ln]

    @line.setter
    def line(self, value):
        self.buffer[self.ln] = value

class hooks:
    move_pre  = NativeHook()
    move_post = NativeHook()

    move_ln   = NativeHook()
    move_cn   = NativeHook()

@hooks.move_ln(500)
def handle_ln_move(cursor, old, new):
    if old != None:
        core.ui.draw_buffer_line(cursor.buffer, old)

    core.ui.draw_buffer_line(cursor.buffer, new)

@hooks.move_cn(500)
def handle_cn_move(cursor, old, new):
    if cursor.ln != None:
        core.ui.draw_buffer_line(cursor.buffer, cursor.ln)

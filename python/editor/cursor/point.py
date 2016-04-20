import core.buffer

from symbols.vec import str2vec

import sys


class Point:
    def __init__(self, buffer):
        self.buffer = buffer

        if len(self.buffer) == 0:
            self.buffer.insert(0)

        self.cn = 0
        self.ln = 0

        @core.buffer.hooks.line.delete_post(500)
        def handle_line_delete(b, ln, v):
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

    def merge_line_back(self):
        if self.ln == 0:
            return

        oldline = self.line

        self.buffer.delete(self.ln)

        self.ln  -= 1

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

        self.line = self.line[:-1]

    def insert(self, string):
        for char in string.encode("ascii"):
            self.insert_char(char)

    def insert_char(self, char):
        l = self.line

        l.insert(self.cn, char)


        print(self.ln, "HIIIIII", file=sys.stderr)
        self.line = l

        self.cn += 1

    def enter(self):
        start = self.line
        start.delete(self.cn, len(start) - self.cn)

        end   = self.line
        end.delete(0, self.cn)

        self.line = start

        self.ln  += 1
        self.cn   = 0
        print("BLEN", len(self.buffer), file=sys.stderr)
        self.buffer.insert(self.ln)
        print("BLEN", len(self.buffer), file=sys.stderr)

        self.line = end

    def correct_colpos(self):
        linelen = len(self.buffer[self.ln])

        if self.cn >= linelen:
            self.cn = linelen

    def correct_linepos(self):
        length = len(self.buffer)
        print("LEN", length, file=sys.stderr)
        if self.ln >= length:
            self.ln = length - 1

    @property
    def line(self):
        return self.buffer[self.ln]

    @line.setter
    def line(self, value):
        self.buffer[self.ln] = value
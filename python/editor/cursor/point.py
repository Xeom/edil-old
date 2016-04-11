import core.buffer

from symbols.vec import str2vec

class Point:
    def __init__(self, buffer):
        self.buffer = buffer
        self.cn     = 0
        self.ln     = 0

        @core.buffer.hooks.line.delete_pre(500)
        def handle_line_delete(b, ln, v):
            if self.ln >= ln.value and self.buffer == b:
                    self.ln += 1

        self.handle_line_delete = handle_line_delete

        @core.buffer.hooks.line.change_pre(500)
        def handle_line_change(b, ln, v):
            if self.ln == ln.value and self.buffer == b:
                self.correct_colpos()

        self.handle_line_change = handle_line_change

        @core.buffer.hooks.line.insert_pre(500)
        def handle_line_insert(b, ln):
                if self.ln >= ln.value and self.buffer == b:
                    self.ln += 1

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

    def delete_forward(self, n):
        while n > len(self.line) - self.cn:
            n -= len(self.line)
            n += self.cn

            l = self.line

            if self.ln >= len(self.buffer) - 1:
                break

            next = self.buffer[self.ln + 1]
            l   += next

            self.line = l
            self.buffer.delete(self.ln + 1)

        l = self.line
        l.delete(self.ln, min(n, len(l) - self.ln))

        self.line = l

    def delete_char(self):
        if not len(self.buffer):
            return

        if self.cn == 0:
            if self.ln == 0:
                return

            l = self.line
            self.buffer.delete(self.ln)
            self.ln -= 1
            prev     = self.line
            prev    += l
            self.line= prev

            self.cn = len(self.line)

            return

        
        self.cn -= 1

        l = self.line
        l.delete(self.cn, 1)
        self.line = l

    def insert_char(self, char):
        if not len(self.buffer):
            self.buffer.insert(0)

        l = self.line
        l.insert(self.cn, char)
        self.line = l

        self.cn += 1

    def insert(self, string):
        if not len(self.buffer):
            self.buffer.insert(0)

        start = self.line
        start.delete(self.cn, len(start) - self.cn)

        end   = self.line
        end.delete(0, self.cn)

        start += str2vec(string)
        start += end

        self.cn += len(string)

        self.line = start

    def enter(self):
        if not len(self.buffer):
            self.buffer.insert(0)
            return

        start = self.line
        start.delete(self.cn, len(start) - self.cn)

        end   = self.line
        end.delete(0, self.cn)

        self.line = start

        self.ln  += 1
        self.cn   = 0
        self.buffer.insert(self.ln)
        self.line = end


    def correct_colpos(self):
        if len(self.buffer) == 0:
            self.cn = 0
            return

        linelen = len(self.buffer[self.ln])

        if self.ln >= linelen:
            self.ln = linelen - 1

    def correct_linepos(self):
        if len(self.buffer) == 0:
            self.ln = 0
            return

        linelen = len(self.buffer[self.ln])

        if self.ln >= linelen:
            self.ln = linelen - 1

    @property
    def line(self):
        return self.buffer[self.ln]

    @line.setter
    def line(self, value):
        self.buffer[self.ln] = value

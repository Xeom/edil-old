from core.buffer import Buffer

import symbols.buffer

def initsys():
    symbols.buffer.point.initsys()

class Point:
    def __init__(self, buffer, ln=0, cn=0):
        self.struct = symbols.buffer.point.init(buffer.struct, ln, cn)

    def move_cols(self, n):
        symbols.buffer.point.move_cols(self.struct, n)

    def move_lines(self, n):
        symbols.buffer.point.move_lines(self.struct, n)

    def delete(self, n):
        symbols.buffer.point.delete(self.struct, n)

    def insert(self, string):
        if isinstance(string, str):
            string = string.encode("ascii")

        symbols.buffer.point.insert(self.struct, string)

    def enter(self):
        symbols.buffer.point.enter(self.struct)

    @property
    def ln(self):
        return symbols.buffer.point.get_ln(self.struct)

    @ln.setter
    def ln(self, ln):
        return symbols.buffer.point.set_ln(self.struct, ln)

    @property
    def cn(self):
        return symbols.buffer.point.get_cn(self.struct)

    @cn.setter
    def cn(self, cn):
        return symbols.buffer.point.set_cn(self.struct, cn)

    @property
    def buffer(self):
        return Buffer(symbols.buffer.point.get_buffer(self.struct))

    def __lt__(self, other):
        return isinstance(other, Point) and \
            other.buffer == self.buffer and \
            symbols.buffer.point.cmp(self.struct, other.struct) == -1

    def __le__(self, other):
        return isinstance(other, Point) and \
            other.buffer == self.buffer and \
            symbols.buffer.point.cmp(self.struct, other.struct) in (-1, 0)

    def __gt__(self, other):
        return isinstance(other, Point) and \
            other.buffer == self.buffer and \
            symbols.buffer.point.cmp(self.struct, other.struct) == 1

    def __gt__(self, other):
        return isinstance(other, Point) and \
            other.buffer == self.buffer and \
            symbols.buffer.point.cmp(self.struct, other.struct) in (1, 0)

    def __eq__(self, other):
        return isinstance(other, Point) and \
            other.buffer == self.buffer and \
            symbols.buffer.point.cmp(self.struct, other.struct) == 0

    def __sub__(self, other):
        return symbols.buffer.point.sub(self.struct, other.struct)

import ctypes

import symbols.buffer

import core.hook
from core.windows import Window
from core.buffer  import Buffer
from symbols.vec  import Vec

def initsys():
    symbols.buffer.deferline.initsys()

class DeferLine:
    def __init__(self, ptr):
        self.struct = ctypes.cast(ptr, symbols.buffer.deferline_p)

    def insert(self, index, string):
        if isinstance(string, str):
            string = string.encode("ascii")

        symbols.buffer.deferline.insert(self.struct, index, string)

    @property
    def vec(self):
        return Vec(symbols.buffer.deferline.get_vec(self.struct), ctypes.c_char)

class hooks:
    draw = core.hook.Hook(
        symbols.buffer.deferline.on_draw,
        Window,
        Buffer,
        symbols.buffer.lineno,
        DeferLine)

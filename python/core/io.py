import symbols.io
import ctypes
import core.hook

class hooks:
    keypress = None

def initsys():
    symbols.io.initsys()

    hooks.keypress = core.hook.Hook(symbols.io.on_keypress, keypress)

class keypress:
    def __init__(self, ptr):
        self.struct = ctypes.cast(ptr, ctypes.POINTER(symbols.io.key_s)).contents

    def __bytes__(self):
        charp = symbols.io.key_str(self.struct)
        rtn   = ctypes.cast(charp, ctypes.c_char_p).value
        symbols.lib.free(charp)

        return rtn

    def __str__(self):
        return self.__bytes__().decode("ascii")

import c.io
import ctypes
import systems.hook

class hooks:
    keypress = None

def initsys():
    c.io.initsys()

    hooks.keypress = systems.hook.Hook(c.io.on_keypress, keypress)

class keypress:
    def __init__(self, ptr):
        self.struct = ctypes.cast(ptr, ctypes.POINTER(c.io.key_s)).contents

    def __bytes__(self):
        charp = c.io.key_str(self.struct)
        rtn   = ctypes.cast(charp, ctypes.c_char_p).value
        c.lib.free(charp)

        return rtn

    def __str__(self):
        return self.__bytes__().decode("ascii")

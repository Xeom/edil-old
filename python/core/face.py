import ctypes

import symbols.ui

cursor_serial = symbols.ui.face.cursor.value

default_bg =  symbols.ui.face.colour_black.value

class Face:
    black   = symbols.ui.face.colour_black.value
    blue    = symbols.ui.face.colour_blue.value
    green   = symbols.ui.face.colour_green.value
    cyan    = symbols.ui.face.colour_cyan.value
    red     = symbols.ui.face.colour_red.value
    magenta = symbols.ui.face.colour_magenta.value
    yellow  = symbols.ui.face.colour_yellow.value
    white   = symbols.ui.face.colour_white.value

    def __init__(self, fg, bg, bright=False, under=False):
        self.struct = symbols.ui.face_s(fg, bg, int(bright), int(under))

    def serialize(self, length):
        charp = symbols.ui.face.serialize(self.struct, length)
        rtn   = ctypes.cast(charp, ctypes.c_char_p).value

        symbols.lib.free(charp)

        return rtn

    def colour(self, string):
        return self.serialize(len(string)) + string

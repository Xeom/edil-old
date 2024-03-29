import ctypes

import core.hook

import symbols.io
import symbols.lib

def initsys():
    symbols.io.key.initsys()

class Key:
    def __init__(self, name, con=False, ign=False, esc=False):
        if isinstance(name, str):
            name = name.encode("ascii")

        if name in aliases:
            self.struct = aliases[name].struct

        else:
            self.struct = symbols.io.key_s(0, name)
            self.ignored = ign
            self.control = con
            self.escaped = esc


    def __bytes__(self):
        charp = symbols.io.key.str(self.struct)
        rtn   = ctypes.cast(charp, ctypes.c_char_p).value

        symbols.lib.free(charp)

        return rtn

    def __str__(self):
        return self.__bytes__().decode("ascii")

    def __hash__(self):
        return int.from_bytes(bytes(self.struct), "little")

    def get_modifier(self, mod):
        mods = ord(self.struct.modifiers)

        return bool(mods & mod)

    def set_modifier(self, mod, value):
        mods = ord(self.struct.modifiers)

        if value:
            mods |= mod

        else:
            mods &= ~mod

        self.struct.modifiers = mods

    @property
    def name(self):
        return self.struct.keyname.decode("ascii")

    @property
    def ignored(self):
        return self.get_modifier(symbols.io.key.ign_mod)

    @ignored.setter
    def ignored(self, value):
        return self.set_modifier(symbols.io.key.ign_mod, value)

    @property
    def control(self):
        return self.get_modifier(symbols.io.key.con_mod)

    @control.setter
    def control(self, value):
        return self.set_modifier(symbols.io.key.con_mod, value)

    @property
    def escaped(self):
        return self.get_modifier(symbols.io.key.esc_mod)

    @ignored.setter
    def escaped(self, value):
        return self.set_modifier(symbols.io.key.esc_mod, value)

aliases = {}
aliases = {
    b"RETURN": Key("J", con=True),
    b"TAB":    Key("I", con=True)
}

class KeyFromPtr(Key):
    def __init__(self, ptr):
        self.struct = ctypes.cast(ptr, ctypes.POINTER(symbols.io.key_s)).contents

class hooks:
    key = core.hook.Hook(symbols.io.key.on_key,
                         KeyFromPtr)

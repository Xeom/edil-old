import ctypes
import weakref

import symbols.io
import symbols.hook
import symbols.vec

from core.container import StructObject, Container
from core.hook import Hook, HookFunct
from core.vec  import Vec, VecFreeOnDel

import core.key

class KeymapFunct(HookFunct):
    def call(self, args, hook):
        pyfunct = self.pyfunct()

        if pyfunct == None:
            self.free()
            return

        argvec  = Vec(args, symbols.vec.vec_p)
        keys    = Vec(argvec[0], symbols.io.key_s)

        pykeys = []

        for index in range(len(keys)):
            k = symbols.vec.item(keys.struct, index)
            pykeys.append(core.key.KeyFromPtr(k))

        pyfunct(pykeys)

class KeymapObj(StructObject):
    PtrType = symbols.io.keymap_p

    def __init__(self, ptr):
        self.functs = []
        super().__init__(ptr)

    def press(self, key):
        return symbols.io.keymap.press(self.struct, key.struct)

    def add(self, *keys):
        v = VecFreeOnDel(symbols.vec.init(ctypes.sizeof(symbols.io.key_s)),
                         symbols.io.key_s)

        for k in keys:
            v.insert(len(v), k.struct)

        symbols.io.keymap.add(self.struct, v.struct)
        hook = symbols.io.keymap.get(self.struct, v.struct)

        def rtn(funct):
            kf = KeymapFunct(self, funct, 500, hook, self.functs)

            return funct

        return rtn

    def clear(self):
        return symbols.io.keymap.clear(self.struct)

    def free(self):
        symbols.io.keymap.free(self.struct)

class KeymapFreeOnDel(KeymapObj):
    def __del__(self):
        self.free()

class KeymapContainer(Container):
    Obj = KeymapObj
    delete_struct = Hook(symbols.io.keymap.on_free,
                         symbols.io.keymap_p)

    def new(self):
        ptr = symbols.io.keymap.init()
        return self(ptr, objtype=KeymapFreeOnDel)

Keymap = KeymapContainer()

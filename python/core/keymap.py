import ctypes
import weakref

import symbols.io
import symbols.hook
import symbols.vec

import core.key

class KeymapFunct:
    def __init__(self, parent, hook, funct):
        self.struct  = hook
        self.parent  = parent
        self.pyfunct = weakref.ref(funct, self.free)
        self.cfunct  = symbols.hook.hook_f(self.call)
        symbols.hook.mount(self.struct, self.cfunct, 500)
        self.parent.functs.append(self)

    def free(self, obj=None):
        symbols.hook.unmount(self.struct, self.cfunct)
        self.parent.functs.remove(self)

    def call(self, args, hook):
        pyfunct = self.pyfunct()

        if pyfunct == None:
            self.free()
            return

        argvec  = symbols.vec.Vec(args, symbols.vec.vec_p)
        keys    = symbols.vec.Vec(argvec[0], symbols.io.key_s)

        pykeys = []

        for index in range(len(keys)):
            k = symbols.vec.item(keys.struct, index)
            pykeys.append(core.key.KeyFromPtr(k))

        pyfunct(pykeys)

class Keymap:
    def __init__(self):
        self.struct = symbols.io.keymap.init()
        self.functs = []

    def press(self, key):
        return symbols.io.keymap.press(self.struct, key.struct)

    def add(self, *keys):
        v = symbols.vec.VecFreeOnDel(
            symbols.vec.init(ctypes.sizeof(symbols.io.key_s)),
            symbols.io.key_s)

        for k in keys:
            v.insert(len(v), k.struct)

        symbols.io.keymap.add(self.struct, v.struct)
        hook = symbols.io.keymap.get(self.struct, v.struct)

        def rtn(funct):
            kf = KeymapFunct(self, hook, funct)

            return funct

        return rtn

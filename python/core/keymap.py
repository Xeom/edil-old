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
    def __init__(self, ptr):
        self.struct = ptr
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

    def clear(self):
        return symbols.io.keymap.clear(self.struct)

class KeyFrame:
    def __init__(self):
        self.maps = []

    def push(self, mapname, priority=0):
        m = maps[mapname]

        self.maps.append(m)

    def press(self, key):
        for map in self.maps:
            if map.press(key) in (1, 2):
                break

    def clear(self):
        for map in self.maps:
            map.clear()

class KeyCont:
    def __init__(self):
        self.items = {}

    def __getitem__(self, name):
        return self.items[name]

class KeyFrameCont(KeyCont):
    def __init__(self):
        KeyCont.__init__(self)
        self.active = None

    def add(self, name):
        if name not in self.items:
            self.items[name] = KeyFrame()

    def switch(self, name):
        self.active = self.items[name]
        self.active.clear()

    def press(self, key):
        if self.active != None:
            self.active.press(key)

class KeyMapCont(KeyCont):
    def __init__(self):
        self.items = {}

    def add(self, name):
        if name not in self.items:
            self.items[name] = Keymap(symbols.io.keymap.init())

frames = KeyFrameCont()
maps   = KeyMapCont()

@core.key.hooks.key(500)
def handle_press(key):
    frames.press(key)

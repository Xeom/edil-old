import symbols.mode

import core.windows

from core.keymap    import Keymap
from core.hook      import Hook
from core.container import Container, StructObject

class NativeModeMountf:
    def __init__(self, nativehook, funct):
        self.hook  = nativehook
        self.funct = funct

    def mount(self):
        self.hook.mount.mount()

    def unmount(self):
        pass

class ModeObj(StructObject):
    PtrType = symbols.mode.mode_p

    def __init__(self, ptr):
        self.mounts = {}
        self.native = []

        self.keymap        = Keymap(symbols.mode.get_keymap(ptr))
        self.on_activate   = Hook(symbols.mode.get_on_activate(ptr))
        self.on_deactivate = Hook(symbols.mode.get_on_deactivate(ptr))

        super().__init__(ptr)

    def activate(self):
        symbols.mode.activate(self.struct)

        for hook, funct in self.native:
            hook.mount(funct)

    def deactivate(self):
        symbols.mode.deactivate(self.struct)

        for hook, funct in self.native:
            hook.unmount(funct)

    def free(self):
        symbols.mode.free(self.struct)
        self.valid = False

    def add_mount(self, hook, funct):
        if isinstance(hook, NativeHook):
            self.native.append([hook, funct])

        else:
            cfunct = hook.generate_cfunct(funct)
            self.mounts.append([hook, funct, cfunct])
            symbols.mode.add_mount(self.struct, hook.struct, cfunct)

    def remove_mount(self, hook, funct):
        if isinstance(hook, NativeHook):
            self.native.remove([hook, funct])

        else:
            ind, cfunct = next(enumerate(
                c for h, f, c in self.mounts if [h, f] == [hook, funct]))

            del self.mounts[ind]

            symbols.mode.remove_mount(self.struct, hook.struct, cfunct)

class ModeFreeOnDel(ModeObj):
    def __del__(self):
        self.free()

class ModeStuck(ModeFreeOnDel):
    modes = {}
    curr  = None

    @classmethod
    def handle_change(cls, new, old):
        cls.curr = new

        for oldmode in cls.modes.get(old, []):
            oldmode.deactivate()

        for newmode in cls.modes.get(new, []):
            newmode.activate()

    @classmethod
    def check_change(cls):
        new = cls.get_sticker()

        if new != cls.curr:
            cls.handle_change(new, cls.curr)

    def __init__(self, ptr):
        super().__init__(ptr)

        @self.on_activate(500)
        def handle_activate():
            item     = self.get_sticker()
            bufmodes = self.modes.get(item)

            if bufmodes == None:
                bufmodes = []
                self.modes[item] = bufmodes

            bufmodes.append(self)

        @self.on_deactivate(500)
        def handle_deactivate():
            item     = self.get_sticker()
            bufmodes = self.modes.get(item)

            if bufmodes != None and self in bufmodes:
                bufmodes.remove(self)

        self.on_activate_handler   = handle_activate
        self.on_deactivate_handler = handle_deactivate

class ModeStuckBuffer(ModeStuck):
    @staticmethod
    def get_sticker():
        return core.windows.get_selected().buffer

@core.windows.hooks.select(200)
@core.windows.hooks.buffer_set(200)
def handle_stuck_buffer(*args):
    ModeStuckBuffer.check_change()

class ModeStuckWindow(ModeStuck):
    @staticmethod
    def get_sticker():
        return core.windows.get_selected()

@core.windows.hooks.delete_post(200)
@core.windows.hooks.split(200)
@core.windows.hooks.select(200)
def handle_stuck_window(*args):
    ModeStuckWindow.check_change()

class ModeStuckCursor(ModeStuck):
    @staticmethod
    def get_sticker():
        return core.cursor.get_selected()

@core.cursor.hooks.select(200)
@core.windows.hooks.select(200)
@core.windows.hooks.delete_post(200)
@core.windows.hooks.split(200)
@core.windows.hooks.buffer_set(200)
def handle_stuck_cursor(*args):
    ModeStuckCursor.check_change()

class ModeContainer(Container):
    Obj           = ModeObj
    delete_struct = Hook(symbols.mode.on_free)

    def new(self, priority, name):
        if isinstance(name, str):
            name = name.encode("ascii")

        ptr = symbols.mode.init(priority, name)
        return self(ptr, objtype=ModeFreeOnDel)

    def new_buffer(self, priority, name):
        if isinstance(name, str):
            name = name.encode("ascii")

        ptr = symbols.mode.init(priority, name)
        return self(ptr, objtype=ModeStuckWindow)

    def new_window(self, priority, name):
        if isinstance(name, str):
            name = name.encode("ascii")

        ptr = symbols.mode.init(priority, name)
        return self(ptr, objtype=ModeStuckWindow)

    def new_cursor(self, priority, name):
        if isinstance(name, str):
            name = name.encode("ascii")

        ptr = symbols.mode.init(priority, name)
        return self(ptr, objtype=ModeStuckCursor)

Mode = ModeContainer()

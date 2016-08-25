import symbols.mode

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

class ModeObject(StructObject):
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

class ModeFreeOnDel(Mode):
    def __del__(self):
        self.free()


class ModeContainer(Container):
    Obj           = ModeObject
    delete_struct = Hook(symbols.mode.on_free)

    def new(self, priority, name):
        ptr = symbols.mode.init(priority, name)
        return self(ptr, objtype=ModeFreeOnDel)

Mode = ModeContainer()

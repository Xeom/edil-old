import symbols.mode

class NativeModeMountf:
    def __init__(self, nativehook, funct):
        self.hook  = nativehook
        self.funct = funct

    def mount(self):
        self.hook.mount.mount()

    def unmount(self):
        pass

class ModeObject(StructObj):
    PtrType = symbols.mode.mode_p

    @classfunction
    def new(cls, priority, name):
        ptr = symbols.mode.init(priority, name)
        return cls(ptr)

    def __init__(self, ptr):
        self.mounts = []
        self.native = []
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
        self.mounts.append([hook, funct])

        if isinstance(hook, NativeHook):
            self.native.append([hook, funct])

        else:
            cfunct = hook.generate_cfunct(funct)
            symbols.mode.add_mount(self.struct, hook.struct, funct)

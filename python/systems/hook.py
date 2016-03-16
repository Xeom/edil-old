import c.hook
import c.vec

import ctypes

class HookFunct:
    def __init__(self, funct, priority):
        self.pyfunct = funct
        self.cfunct  = c.hook.hook_f(self.call, priority)

        c.hook.mount(self.struct, self.cfunct)

    def call(self, args, hook):
        cargs = c.vec.Vec(args, ctypes.c_void_p)
        pyargs = []

        for type, arg in zip(self.types, cargs):
            value = ctypes.POINTER(type)(arg).contents
            pyargs.append(value)

        try:
            self.pyfunct(*pyargs)
        except:
            pass # TODO: Something

    def free(self):
        c.hook.unmount(self.struct, self.cfunct);

    def __del__(self):
        self.free()

class Hook:
    def __init__(self, struct, *types):
        self.types   = types
        self.struct  = struct
        self.functs  = []

    def free(self):pass

    def __call__(self, priority=0):
        return lambda funct:self.mount(funct)

    def mount(self, funct, priority):
        self.functs.append(HookFunction(funct))


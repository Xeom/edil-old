import c.hook
import c.vec

import ctypes

class HookFunct:
    def __init__(self, struct, funct, priority):
        self.struct  = struct
        self.pyfunct = funct
        self.cfunct  = c.hook.hook_f(funct)

        c.hook.mount(self.struct, self.cfunct, priority)

    def free(self):
        c.hook.unmount(self.struct, self.cfunct);

    def __del__(self):
        self.free()

class Hook:
    def __init__(self, struct, *types):
        self.types  = types
        self.struct = struct
        self.functs = []

    def __call__(self, priority=0):
        return lambda funct:self.mount(funct, priority)

    def mount(self, funct, priority):
        self.functs.append(HookFunct(self.struct, self.wrap(funct), priority))

        return funct

    def wrap(self, pyfunct):
        def f(args, hook):
            import sys

            cargs = c.vec.Vec(args, ctypes.c_void_p)
            pyargs = []

            for type, arg in zip(self.types, cargs):
                value = ctypes.cast(arg, ctypes.POINTER(type)).contents
                pyargs.append(value)

            print(pyargs)
#            try:
            pyfunct(*pyargs)
 #           except:
  #              pass # TODO: Something

        return f

import c.hook
import c.vec

import ctypes

class Hook:
    def __init__(self, struct, *types):
        self.types   = types
        self.struct  = struct
        self.functs  = []

    def free(self):pass

    def __call__(self):
        self.mount(function)

    def mount(self, function):
        def f(args, hook):
            import sys

            cargs = c.vec.Vec(args, ctypes.c_void_p)
            print([i for i in iter(cargs)], file=sys.stderr)
            pyargs = []

            for type, arg in zip(self.types, cargs):
                value = ctypes.POINTER(type)(arg).contents
                pyargs.append(value)

            function(*pyargs)

        c.hook.mount(self.struct, c.hook.callback_f(f))
        self.functs.append(f)

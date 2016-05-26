import ctypes

import symbols.callback

class CallbackFunct:
    def __init__(self, struct, funct):
        self.struct  = struct
        self.pyfunct = funct
        self.cfunct  = symbols.callback.callback_f(funct)

        symbols.callback.mount(self.struct, self.cfunct)

    def free(self):
        symbols.callback.unmount(self.struct)

    def __del__(self):
        self.free()

class Callback:
    def __init__(self, struct, restype, *types):
        self.restype = restype
        self.types   = types
        self.struct  = struct
        self.funct   = None

    def __call__(self):
        return lambda funct:self.mount(funct)

    def mount(self, funct):
        self.funct = CallbackFunct(self.struct, self.wrap(funct))

    def wrap(self, pyfunct):
        def f(args, cb):
            cargs  = symbols.vec.Vec(args, ctypes.c_void_p)
            pyargs = []

            for type, arg in zip(self.types, cargs):
                value =  ctypes.POINTER(type)(arg).contents
                pyargs.append(value)

#            try:
            return ctypes.cast(self.restype(pyfunct(*pyargs)), ctypes.c_void_p).value
#            except:
#                return 0

        return f

import symbols.hook
import symbols.vec

import ctypes
import weakref

import sys

# A class for containing a python function attached to a hook
class HookFunct:
    def __init__(self, parent, funct, priority):
        self.parent  = parent
        self.struct  = parent.struct
        # By using a weakref, we do not keep the hooked function
        # alive and can tell when it has died.
        #
        # Use self.free as a callback for when funct dies.
        self.pyfunct = weakref.ref(funct, self.free)
        # Wrap up self.call with ctypes
        self.cfunct  = symbols.hook.hook_f(self.call)

        symbols.hook.mount(self.struct, self.cfunct, priority)

    # Unmounts the function from its hook and removes the reference
    # to this hookfunct from the appropriate hook, allowing it to die.
    def free(self, obj=None):
        symbols.hook.unmount(self.struct, self.cfunct)
        self.parent.functs.remove(self)

    # The python function wrapped up to mount to hook_mount()
    def call(self, args, hook):
        # Rereference the weakref
        pyfunct = self.pyfunct()

        # If our function has died, free ourselves
        if pyfunct == None:
            self.free()
            return

        # Cast our argument vector to a nice Vec class
        cargs = symbols.vec.Vec(args, ctypes.c_void_p)
        pyargs = []

        # Extract arguments and cast them to their correct types
        for type, arg in zip(self.parent.types, cargs):
            if issubclass(type, ctypes.Structure)    or \
               issubclass(type, ctypes._SimpleCData) or \
               issubclass(type, ctypes._Pointer):
                # If the type is a raw ctype, then our argument is a ptr
                # to that type, and we dereference it.
                value = ctypes.cast(arg, ctypes.POINTER(type)).contents

            else:
                # Otherwise, we dereference our argument to a void*
                value = type(ctypes.cast(arg, ctypes.POINTER(ctypes.c_void_p)).contents)

            pyargs.append(value)

        try:
            pyfunct(*pyargs)
        except:
            pass # TODO: Something

class Hook:
    def __init__(self, struct, *types):
        self.types  = types
        self.struct = struct
        self.functs = []

    def __call__(self, priority=0):
        if not isinstance(priority, int):
            raise Exception("Priority must be an integer")

        return lambda funct:self.mount(funct, priority)

    def mount(self, funct, priority):
        if isinstance(funct, HookFunct):
            funct = HookFunct.pyfunct

        rtn = HookFunct(self, funct, priority)

        self.functs.append(rtn)

        return funct

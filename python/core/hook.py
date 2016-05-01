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
        self.parent.functs.append(self)

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
                # Otherwise, we simply hand the pointer to the class
                value = type(arg)

            pyargs.append(value)

  #      try:
        pyfunct(*pyargs)
#        except:
 #           pass # TODO: Something

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
        hf = HookFunct(self, funct, priority)

        if hasattr(funct, "__hookfunct__"):
            funct.__hookfunct__.append(hf)

        else:
            funct.__hookfunct__ = [hf]

        return funct

class NativeHookFunct:
    def __init__(self, parent, funct, priority):
        self.parent = parent
        self.pyfunct = weakref.ref(funct, self.free)

        for index, val in parent.functs:
            if val.priority <= priority:
                self.parent.functs.insert(index, self)
                return

        self.parent.functs.append(self)

    def free(self, obj=None):
        self.parent.functs.remove(self)

    def call(self, args):
        pyfunct = self.pyfunct()

        if pyfunct == None:
            self.free()
            return

        pyfunct(*args)

class NativeHook:
    def __init__(self):
        self.functs = []

    def __call__(self, priority=0):
        if not isinstance(priority, int):
            raise Exception("Priority must be an integer")

        return lambda funct:self.mount(funct, priority)

    def mount(self, funct, priority):
        NativeHookFunct(self, funct, priority)

        return funct

    def call(self, args):
        for funct in self.functs:
            funct.call(args)

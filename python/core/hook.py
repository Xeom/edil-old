import ctypes
import weakref

import sys

import symbols.hook

from core.vec import Vec

# A class for containing a python function attached to a hook
class HookFunct:
    """A class to contain and wrap a python function associated with a hook.

    This class provides a wrapping around the python function, which can be used
    in a ctypes function pointer and mounted directly to the C hook. The wrapper
    extracts arguments from the vec handed to the C hook, and initializes them
    into classes.
    """

    def __init__(self, parent, funct, priority, hook, functs=None):
        """Initialize an instance of this class.

        Arguments:
            parent   (Hook): The Hook that this function is hooked to.
            funct    (function): The python function to be hooked to the hook.
            priority (int): The priority of this funct - a value from 0 to 1000.
        """
        self.parent  = parent
        self.struct  = hook
        self.functs  = functs

        # By using a weakref, we do not keep the hooked function alive.
        # Use self.free as a callback for when funct dies.
        self.pyfunct = weakref.ref(funct, self.free)

        # Wrap up self.call with ctypes to make a C function pointer.
        self.cfunct = symbols.hook.hook_f(self.call)

        # Mount the C function pointer on the appropriate hook.
        symbols.hook.mount(self.struct, self.cfunct, priority)

        # Keep a reference to this class in the parent's list of functions. This
        # way it will be kept alive until the weakly referenced funtion dies.
        if self.functs != None:
            self.functs.append(self)

    def free(self, obj=None):
        """Handles the deletion of this function.

        Unmounts the function from its hook, and removes the reference to this
        class from its parent, committing suicide and cleaning up the HookFunct.

        Optional Arguments:
            obj (...): This argument is simply present so that if this function
                is called by a weakref callback, it can accept the weakref
                object that will be passed to it.
        """

        symbols.hook.unmount(self.struct, self.cfunct)

        if self.functs != None and self in self.functs:
            self.functs.remove(self)

    # The python function wrapped up to mount to hook_mount()
    def call(self, args, hook):
        """Function to be made into a C function pointer.

        When called, extracts python arguments from its arguments, and uses them
        to call the python function associated with this class.

        Arguments:
            args (vec_p): A pointer to a vector containing arguments for a call
                to this hook function.
            hook (hook_p): A pointer to the hook this function is hooked to,
                (unused in this function).
        """

        # Dereference the weakref
        pyfunct = self.pyfunct()

        # If our function has died, free ourselves. This should never happen, as
        # .free() should have been called from a callback, but whatever, this
        # class might not have been GC'd.
        if pyfunct == None:
            self.free()
            return

        # Cast our argument vector to a nice Vec class
        cargs = Vec(args, ctypes.c_void_p)
        pyargs = []

        # Extract arguments and cast them to their correct types
        for typ, arg in zip(self.parent.types, cargs):
            if isinstance(typ, type) and (             \
               issubclass(typ, ctypes.Structure)    or \
               issubclass(typ, ctypes._SimpleCData) or \
               issubclass(typ, ctypes._Pointer)):
                # If the type is a raw ctype, then our argument is a ptr
                # to that type, and we dereference it.
                value = ctypes.cast(arg, ctypes.POINTER(typ)).contents

            else:
                # Otherwise, we simply hand the pointer to the class
                value = typ(arg)

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
        hf = HookFunct(self, funct, priority, self.struct, self.functs)

        if hasattr(funct, "_hookfunct"):
            funct._hookfunct.append(hf)

        else:
            funct._hookfunct = [hf]

        return funct

class NativeHookFunct:
    def __init__(self, parent, funct, priority):
        self.parent  = parent
        self.pyfunct = weakref.ref(funct, self.free)
        self.priority = priority

        for index, val in enumerate(parent.functs):
            if val.priority <= priority:
                self.parent.functs.insert(index, self)
                return

        self.parent.functs.append(self)

    def free(self, obj=None):
        if self in self.parent.functs:
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

    def mount_gethookfunct(self, funct, priority):
        return NativeHookFunct(self, funct, priority)

    def call(self, args):
        for funct in self.functs:
            funct.call(args)

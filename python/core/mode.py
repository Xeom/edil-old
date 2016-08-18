class NativeModeMountf:
    def __init__(self, nativehook, funct):
        self.hook  = nativehook
        self.funct = funct

    def mount(self):
        self.hook.mount.mount()

    def unmount(self):
        pass


def generate_mount_funct(argtypes, funct):
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

        # Cast our argument vector to a nice Vec class
        cargs = Vec(args, ctypes.c_void_p)
        pyargs = []

        # Extract arguments and cast them to their correct types
        for typ, arg in zip(argtypes, cargs):
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

        pyfunct(*pyargs)

    cfunct = symbols.hook.hook_f(self.call)

    return cfunct

class ModeObject(ContainerObj):
    PtrType = symbols.mode.mode_p

    def __init__(self):
        self.nativemounts = []

    def activate(self):
        symbols.mode.activate(self.struct)

    def deactivate(self):
        symbols.mode.deactivate(self.struct)

    def add_mount(self, hook, funct):
        if isinstance(hook, NativeHook):
            self.nativemounts.append([hook, funct])lw

        else:
            symbols.mode.add_mount(self.struct, hook.struct, funct)

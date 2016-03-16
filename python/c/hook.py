import ctypes
import c.vec
from shared import lib as so

class hook_s(ctypes.Structure):
    pass

priority = ctypes.c_ushort

hook_p = ctypes.POINTER(hook_s)

hook_f = ctypes.CFUNCTYPE(None, c.vec.vec_p, hook_p)

init = so.hook_init
init.argtypes = []
init.restype  = hook_p

mount = so.hook_mount
mount.argtypes = [hook_p, hook_f, priority]
mount.restype  = ctypes.c_int

call = so.hook_call



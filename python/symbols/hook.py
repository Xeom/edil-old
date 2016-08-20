import ctypes

from symbols.vec import vec_p
from shared import lib as so

class hook_s(ctypes.Structure):
    _fields_ = [("functs", vec_p),
                ("numargs", ctypes.c_size_t)]

priority = ctypes.c_ushort

hook_p = ctypes.POINTER(hook_s)

hook_f = ctypes.CFUNCTYPE(None, vec_p, hook_p)

killsys = so.hook_killsys
killsys.argtypes = []
killsys.restype  = ctypes.c_int

mount = so.hook_mount
mount.argtypes = [hook_p, hook_f, priority]
mount.restype  = ctypes.c_int

unmount = so.hook_unmount
unmount.argtypes = [hook_p, hook_f]
unmount.restype  = ctypes.c_int

call = so.hook_call
# Takes varargs, risky to use


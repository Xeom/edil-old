import ctypes
from shared import lib as so

from symbols.vec import vec_p

class callback_s(ctypes.Structure):
    pass

callback_f = ctypes.CFUNCTYPE(ctypes.c_void_p, vec_p, callback_s)

callback_s._fields_ = [("funct", callback_f),
                       ("numargs", ctypes.c_size_t)]

callback_p = ctypes.POINTER(callback_s)

mount = so.callback_mount
mount.argtypes = [callback_p, callback_f]

import ctypes
from shared import lib as so

from hook import hook_f

class mode_s(ctypes.Structure):
    pass

mode_p = ctypes.POINTER(mode_s)

init = so.mode_init
init.argtypes = [ctypes.c_int, ctypes.c_char_p]
init.restype  = mode_p

activate = so.mode_activate
activate.argtypes = [mode_p]
activate.restype  = ctypes.c_int

deactivate = so.mode_deactivate
deactivate.argtypes = [mode_p]
deactivate.restype  = ctypes.c_int

handle_press = so.mode_handle_press
handle_press.argtypes = [key_s]
handle_press.restype  = ctypes.c_int

free = so.mode_free
free.argtypes = [mode_p]
free.restype  = ctypes.c_int

add_mount = so.mode_add_monut
add_mount.argtypes = [mode_p, hook_p, hook_f]
add_mount.restype  = ctypes.c_int

remove_mount = so.mode_remove_monut
remove_mount.argtypes = [mode_p, hook_p, hook_f]
remove_mount.restype  = ctypes.c_int


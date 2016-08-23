import ctypes

from symbols.hook import hook_p, hook_f
from symbols.io   import key_s
from shared import lib as so

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

on_activate = so.mode_on_activate
on_activate.argtypes = [mode_p]
on_activate.restype  = hook_p

on_deactivate = so.mode_on_deactivate
on_deactivate.argtypes = [mode_p]
on_deactivate.restype  = hook_p

handle_press = so.mode_handle_press
handle_press.argtypes = [key_s]
handle_press.restype  = ctypes.c_int

free = so.mode_free
free.argtypes = [mode_p]
free.restype  = ctypes.c_int

add_mount = so.mode_add_mount
add_mount.argtypes = [mode_p, hook_p, hook_f]
add_mount.restype  = ctypes.c_int

remove_mount = so.mode_remove_mount
remove_mount.argtypes = [mode_p, hook_p, hook_f]
remove_mount.restype  = ctypes.c_int

on_free = ctypes.cast(so.mode_on_free, hook_p)
on_init = ctypes.cast(so.mode_on_init, hook_p)

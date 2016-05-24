import ctypes
from shared import lib as so

from symbols.hook   import hook_p
from symbols.vec    import vec_p
from symbols.buffer import buffer_p


class listener_s(ctypes.Structure):
    pass

listener_p = ctypes.POINTER(listener_s)

class key_s(ctypes.Structure):
    _fields_ = [("modifiers", ctypes.c_char),
                ("keyname",   ctypes.c_char *
                 ctypes.cast(so.io_key_name_len, ctypes.POINTER(ctypes.c_int)).contents.value)]

class keymap_s(ctypes.Structure):
    pass

keymap_p = ctypes.POINTER(keymap_s)

listenf_none_f = ctypes.CFUNCTYPE(listener_p)
listenf_char_f = ctypes.CFUNCTYPE(listener_p, ctypes.c_char)
listenf_str_f  = ctypes.CFUNCTYPE(listener_p, ctypes.POINTER(ctypes.c_char), ctypes.c_size_t)

class key:
    set_name = so.io_key_set_name
    set_name.argtypes = [key_s, ctypes.c_char_p]
    set_name.restype  = key_s

    initsys = so.io_key_initsys
    initsys.argtypes = []
    initsys.restype  = ctypes.c_int

    killsys = so.io_key_killsys
    killsys.argtypes = []
    killsys.restype  = ctypes.c_int

    str = so.io_key_str
    str.argtypes = [key_s]
    str.restype  = ctypes.POINTER(ctypes.c_char)

    handle_chr = so.io_key_handle_chr
    handle_chr.argtypes = [ctypes.c_int]
    handle_chr.restype  = ctypes.c_int

    resize   = ctypes.cast(so.io_key_resize,   ctypes.POINTER(ctypes.c_int)).contents.value
    name_len = ctypes.cast(so.io_key_name_len, ctypes.POINTER(ctypes.c_int)).contents.value

    on_key = ctypes.cast(so.io_key_on_key, hook_p)

    ign_mod = ord(ctypes.cast(so.io_key_ign_mod, ctypes.POINTER(ctypes.c_char))[0])
    esc_mod = ord(ctypes.cast(so.io_key_esc_mod, ctypes.POINTER(ctypes.c_char))[0])
    con_mod = ord(ctypes.cast(so.io_key_con_mod, ctypes.POINTER(ctypes.c_char))[0])

class file:
    dump_vec = so.file_dump_vec
    dump_vec.argtypes = [vec_p, ctypes.c_void_p]
    dump_vec.restype  = ctypes.c_int

    dump_buffer = so.file_dump_buffer
    dump_buffer.argtypes = [buffer_p, ctypes.c_void_p]
    dump_buffer.restype  = ctypes.c_int

    read_buffer = so.file_read_buffer
    read_buffer.argtypes = [buffer_p, ctypes.c_void_p]
    read_buffer.restype  = ctypes.c_int

class keymap:
    init = so.keymap_init
    init.argtypes = []
    init.restype  = keymap_p

    clear = so.keymap_clear
    clear.argtypes = [keymap_p]

    press = so.keymap_press
    press.argtypes = [keymap_p, key_s]
    press.restype  = ctypes.c_int

    get_unknown = so.keymap_get_unknown
    get_unknown.argtypes = [keymap_p]
    get_unknown.restype  = hook_p

    get = so.keymap_get
    get.argtypes = [keymap_p, vec_p]
    get.restype  = hook_p

    add = so.keymap_add
    add.argtypes = [keymap_p, vec_p]
    add.restype  = ctypes.c_int

    delete = so.keymap_delete
    delete.argtypes = [keymap_p, vec_p]
    delete.restype  = ctypes.c_int

class listener:
    initsys = so.io_listener_initsys
    initsys.argtypes = []
    initsys.restype  = ctypes.c_int

    listen = so.io_listener_listen
    listen.argtypes = []
    listen.restype  = ctypes.c_int

    init = so.io_listener_init
    init.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_size_t,
                     listenf_char_f, listenf_str_f, listenf_none_f]
    init.restype  = listener_p

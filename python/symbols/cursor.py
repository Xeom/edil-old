import ctypes
from shared import lib as so

from symbols.buffer import buffer_p, buffer_s, lineno, colno
from symbols.hook   import hook_p
from symbols.vec    import vec_p

class cursor_s(ctypes.Structure):
    pass

cursor_p = ctypes.POINTER(cursor_s)

class cursor_type_s(ctypes.Structure):
    _fields_ = [
        ("init",
         ctypes.CFUNCTYPE(ctypes.c_void_p, buffer_p)),
        ("free",
         ctypes.CFUNCTYPE(ctypes.c_int,    ctypes.c_void_p)),

        ("get_ln",
         ctypes.CFUNCTYPE(lineno,   ctypes.c_void_p)),
        ("get_cn",
         ctypes.CFUNCTYPE(colno,    ctypes.c_void_p)),

        ("set_ln",
         ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_void_p, lineno)),
        ("set_cn",
         ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_void_p, colno)),

        ("move_lines",
         ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_void_p, ctypes.c_int)),
        ("move_cols",
         ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_void_p, ctypes.c_int)),

        ("insert",
         ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_void_p, ctypes.c_char_p)),
        ("delete",
          ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_void_p, ctypes.c_uint)),
        ("enter",
         ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_void_p)),

        ("activate",
         ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_void_p)),
        ("deactivate",
         ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_void_p))
    ]

cursor_type_p = ctypes.POINTER(cursor_type_s)

snap_blacklist = ctypes.cast(so.cursor_snap_blacklist, vec_p)

initsys = so.cursor_initsys
initsys.argtypes = []
initsys.restype  = ctypes.c_int

spawn = so.cursor_spawn
spawn.argtypes = [buffer_p, cursor_type_p]
spawn.restype  = cursor_p

free = so.cursor_free
free.argtypes = [cursor_p]
free.restype  = ctypes.c_int

get_buffer = so.cursor_get_buffer
get_buffer.argtypes = [cursor_p]
get_buffer.restype  = buffer_p

get_ptr = so.cursor_get_ptr
get_ptr.argtypes = [cursor_p]
get_ptr.restype  = ctypes.c_void_p

get_ln = so.cursor_get_ln
get_ln.argtypes = [cursor_p]
get_ln.restype  = lineno

get_cn = so.cursor_get_cn
get_cn.argtypes = [cursor_p]
get_cn.restype  = colno

set_ln = so.cursor_set_ln
set_ln.argtypes = [cursor_p, lineno]
set_ln.restype  = ctypes.c_int

set_cn = so.cursor_set_cn
set_cn.argtypes = [cursor_p, colno]
set_cn.restype  = ctypes.c_int

move_cols = so.cursor_move_cols
move_cols.argtypes = [cursor_p, ctypes.c_int]
move_cols.restype  = ctypes.c_int

move_lines = so.cursor_move_lines
move_lines.argtypes = [cursor_p, ctypes.c_int]
move_lines.restype  = ctypes.c_int

insert = so.cursor_insert
insert.argtypes = [cursor_p, ctypes.c_char_p]
insert.restype  = ctypes.c_int

delete = so.cursor_delete
delete.argtypes = [cursor_p, ctypes.c_uint]
delete.restype  = ctypes.c_int

enter = so.cursor_enter
enter.argtypes = [cursor_p]
enter.restype  = ctypes.c_int

activate = so.cursor_activate
activate.argtypes = [cursor_p]
activate.restype  = ctypes.c_int

deactivate = so.cursor_deactivate
deactivate.argtypes = [cursor_p]
deactivate.restype  = ctypes.c_int

buffer_selected = so.cursor_buffer_selected
buffer_selected.argtypes = [buffer_p]
buffer_selected.restype  = cursor_p

buffer_all = so.cursor_buffer_all
buffer_all.argtypes = [buffer_p]
buffer_all.restype  = vec_p

selected = so.cursor_selected
selected.argtypes = []
selected.restype  = cursor_p

select_last = so.cursor_select_last
select_last.argtypes = [buffer_p]
select_last.restype  = ctypes.c_int

select_next = so.cursor_select_next
select_next.argtypes = [buffer_p]
select_next.restype  = ctypes.c_int

class point:
    initsys = so.cursor_point_initsys
    initsys.argtypes = []
    initsys.restype  = ctypes.c_int

    type = ctypes.cast(so.cursor_point_type, cursor_type_p)

class region:
    initsys = so.cursor_region_initsys
    initsys.argtypes = []
    initsys.restype  = ctypes.c_int

    type = ctypes.cast(so.cursor_region_type, cursor_type_p)

on_spawn           = ctypes.cast(so.cursor_on_spawn,           hook_p)
on_free            = ctypes.cast(so.cursor_on_free,            hook_p)
on_set_ln_pre      = ctypes.cast(so.cursor_on_set_ln_pre,      hook_p)
on_set_ln_post     = ctypes.cast(so.cursor_on_set_ln_post,     hook_p)
on_set_cn_pre      = ctypes.cast(so.cursor_on_set_cn_pre,      hook_p)
on_set_cn_post     = ctypes.cast(so.cursor_on_set_cn_post,     hook_p)
on_move_lines_pre  = ctypes.cast(so.cursor_on_move_lines_pre,  hook_p)
on_move_lines_post = ctypes.cast(so.cursor_on_move_lines_post, hook_p)
on_move_cols_pre   = ctypes.cast(so.cursor_on_move_cols_pre,   hook_p)
on_move_cols_post  = ctypes.cast(so.cursor_on_move_cols_post,  hook_p)
on_insert_pre      = ctypes.cast(so.cursor_on_insert_pre,      hook_p)
on_insert_post     = ctypes.cast(so.cursor_on_insert_post,     hook_p)
on_delete_pre      = ctypes.cast(so.cursor_on_delete_pre,      hook_p)
on_delete_post     = ctypes.cast(so.cursor_on_delete_post,     hook_p)
on_enter_pre       = ctypes.cast(so.cursor_on_enter_pre,       hook_p)
on_enter_post      = ctypes.cast(so.cursor_on_enter_post,      hook_p)
on_change_pos      = ctypes.cast(so.cursor_on_change_pos,      hook_p)

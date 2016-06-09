import ctypes
from shared import lib as so

from symbols.buffer import buffer_p, lineno, colno
from symbols.vec    import vec_p

class cursor_s(ctypes.Structure):
    pass

cursor_p = ctypes.POINTER(cursor_s)

class cursor_type_s(ctypes.Structure):
    pass

cursor_type_p = ctypes.POINTER(cursor_type_s)

initsys = so.cursor_initsys
initsys.argtypes = []
initsys.restype  = ctypes.c_int

spawn = so.cursor_spawn
spawn.argtypes = [buffer_p, cursor_type_p]
spawn.restype  = cursor_p

get_ln = so.cursor_get_ln
get_ln.argtypes = [cursor_p]
get_ln.restype  = lineno

get_cn = so.cursor_get_cn
get_cn.argtypes = [cursor_p]
get_cn.restype  = colno

get_buffer = so.cursor_get_buffer
get_buffer.argtypes = [cursor_p]
get_buffer.restype  = buffer_p

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

buffer_selected = so.cursor_buffer_selected
buffer_selected.argtypes = [buffer_p]
buffer_selected.restype  = cursor_p

buffer_all = so.cursor_buffer_all
buffer_all.argtypes = [buffer_p]
buffer_all.restype  = vec_p

selected = so.cursor_selected
selected.argtypes = []
selected.restype  = cursor_p

class point:
    initsys = so.cursor_point_initsys
    initsys.argtypes = []
    initsys.restype  = ctypes.c_int
    
    type = ctypes.cast(so.cursor_point_type, cursor_type_p)

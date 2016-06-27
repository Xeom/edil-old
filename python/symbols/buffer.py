import ctypes
from shared import lib as so

from symbols.hook  import hook_p
from symbols.vec   import vec_p
from symbols.table import table_p

class flag:
    readonly = 1
    modified = 2

class buffer_s(ctypes.Structure):
    pass

buffer_p = ctypes.POINTER(buffer_s)

class deferline_s(ctypes.Structure):
    pass

deferline_p = ctypes.POINTER(deferline_s)

class point_s(ctypes.Structure):
    pass

point_p = ctypes.POINTER(point_s)

lineno = ctypes.c_size_t
colno = ctypes.c_size_t

on_batch_region = ctypes.cast(so.buffer_on_batch_region, hook_p)

on_create = ctypes.cast(so.buffer_on_create, hook_p)
on_delete = ctypes.cast(so.buffer_on_delete, hook_p)

from symbols.cursor import cursor_p


class log:
    initsys = so.buffer_log_initsys
    initsys.argtypes = []
    initsys.restype  = ctypes.c_int

    point_stream = so.buffer_log_point_stream
    point_stream.argtypes = [cursor_p]
    point_stream.restype  = ctypes.c_void_p

class line:
    on_change_pre  = ctypes.cast(so.buffer_line_on_change_pre,  hook_p)
    on_change_post = ctypes.cast(so.buffer_line_on_change_post, hook_p)

    on_delete_pre  = ctypes.cast(so.buffer_line_on_delete_pre,  hook_p)
    on_delete_post = ctypes.cast(so.buffer_line_on_delete_post, hook_p)

    on_insert_pre  = ctypes.cast(so.buffer_line_on_insert_pre,  hook_p)
    on_insert_post = ctypes.cast(so.buffer_line_on_insert_post, hook_p)

class point:
    initsys = so.buffer_point_initsys
    initsys.argtypes = []
    initsys.restype  = ctypes.c_int

    init = so.buffer_point_init
    init.argtypes = [buffer_p, lineno, colno]
    init.restype  = point_p

    move_cols = so.buffer_point_move_cols
    move_cols.argtypes = [point_p, ctypes.c_int]
    move_cols.restype  = ctypes.c_int

    move_lines = so.buffer_point_move_lines
    move_lines.argtypes = [point_p, ctypes.c_int]
    move_lines.restype  = ctypes.c_int

    delete = so.buffer_point_delete
    delete.argtypes = [point_p, ctypes.c_uint]
    delete.restype  = ctypes.c_int

    insert = so.buffer_point_insert
    insert.argtypes = [point_p, ctypes.c_char_p]
    insert.restype  = ctypes.c_int

    enter = so.buffer_point_enter
    enter.argtypes = [point_p]
    enter.restype  = ctypes.c_int

    get_ln = so.buffer_point_get_ln
    get_ln.argtypes = [point_p]
    get_ln.restype  = lineno

    set_ln = so.buffer_point_set_ln
    set_ln.argtypes = [point_p, lineno]

    get_cn = so.buffer_point_get_cn
    get_cn.argtypes = [point_p]
    get_cn.restype  = colno

    set_cn = so.buffer_point_set_cn
    set_cn.argtypes = [point_p, colno]

    get_buffer = so.buffer_point_get_buffer
    get_buffer.argtypes = [point_p]
    get_buffer.restype  = buffer_p

    cmp = so.buffer_point_cmp
    cmp.argtypes = [point_p, point_p]
    cmp.restype  = ctypes.c_int

    sub = so.buffer_point_sub
    sub.argtypes = [point_p, point_p]
    sub.restype  = ctypes.c_long

    on_move_pre  = ctypes.cast(so.buffer_point_on_move_pre,  hook_p)
    on_move_post = ctypes.cast(so.buffer_point_on_move_post, hook_p)

class deferline:
    initsys = so.buffer_deferline_initsys
    initsys.argtypes = []
    initsys.restype  = ctypes.c_int

    insert = so.buffer_deferline_insert
    insert.argtypes = [deferline_p, ctypes.c_size_t, ctypes.c_char_p]
    insert.restype  = ctypes.c_int

    delete = so.buffer_deferline_delete
    delete.argtypes = [deferline_p, ctypes.c_size_t]
    delete.restype  = ctypes.c_int

    get_vec = so.buffer_deferline_get_vec
    get_vec.argtypes = [deferline_p]
    get_vec.restype  = vec_p

    on_draw = ctypes.cast(so.buffer_deferline_on_draw, hook_p)

init = so.buffer_init
init.argtypes = []
init.restype  = buffer_p

free = so.buffer_free
free.argtypes = [buffer_p]
free.restype  = None

batch_start = so.buffer_batch_start
batch_start.argtypes = [buffer_p]
batch_start.restype  = ctypes.c_int

batch_end = so.buffer_batch_end
batch_end.argtypes = [buffer_p]
batch_end.restype  = ctypes.c_int

insert = so.buffer_insert
insert.argtypes = [buffer_p, lineno]
insert.restype  = ctypes.c_int

delete = so.buffer_delete
delete.argtypes = [buffer_p, lineno]
delete.restype  = ctypes.c_int

get_line = so.buffer_get_line
get_line.argtypes = [buffer_p, lineno]
get_line.restype  = vec_p

set_line = so.buffer_set_line
set_line.argtypes = [buffer_p, lineno, vec_p]
set_line.restype  = ctypes.c_int

len = so.buffer_len
len.argtypes = [buffer_p]
len.restype  = lineno

get_properties = so.buffer_get_properties
get_properties.argtypes = [buffer_p]
get_properties.restype  = table_p

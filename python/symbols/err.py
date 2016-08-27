import ctypes
from shared import lib as so

import cutil

class err_lvl(ctypes.c_int):
    low      = 1
    medium   = 2
    high     = 3
    critical = 4
    terminal = 5

class CException(Exception):
    def __init__(self, lvl):
        self.lvl = lvl

def gen_cfunct_tracer(lamb, msg=""):
    if isinstance(msg, str):
        msg = msg.encode("ascii")

    def rtn(val, funct, args):
        if not lamb(val):
            lvl = last_lvl.contents

            new(lvl, b"C function return value in Python erroneous",
                msg + b" " + str(funct).encode("ascii"))

            raise CException(lvl)

        return val

    return rtn

cfunct_tracer_int = gen_cfunct_tracer(lambda rtn:rtn != -1,
                                      msg="Function returned -1")
cfunct_tracer_ptr = gen_cfunct_tracer(lambda rtn:not cutil.isnull(rtn),
                                      msg="Function returned NULL")
cfunct_tracer_ind = gen_cfunct_tracer(lambda rtn:rtn != (
    1 << ctypes.sizeof(ctypes.c_size_t) * 8) - 1,
                                      msg="Function returned INVALID_INDEX")

stream = ctypes.cast(so.err_stream, ctypes.POINTER(ctypes.c_void_p))

#log_buffer = ctypes.cast(so.err_log_buffer, ctypes.POINTER(buffer_p))
#log_cursor = ctypes.cast(so.err_log_cursor, ctypes.POINTER(cursor_p))

max_per_sec = ctypes.cast(so.err_max_per_sec, ctypes.POINTER(ctypes.c_uint))

min_quit_lvl   = ctypes.cast(so.err_min_quit_lvl,   ctypes.POINTER(err_lvl))
min_care_lvl   = ctypes.cast(so.err_min_care_lvl,   ctypes.POINTER(err_lvl))
min_detail_lvl = ctypes.cast(so.err_min_detail_lvl, ctypes.POINTER(err_lvl))

last_lvl = ctypes.cast(so.err_last_lvl, ctypes.POINTER(err_lvl))

new = so.err_new
new.argtypes = [err_lvl, ctypes.c_char_p, ctypes.c_char_p]

create_log_buffer = so.err_create_log_buffer
create_log_buffer.argtypes = []
create_log_buffer.restype  = ctypes.c_int
create_log_buffer.errcheck = cfunct_tracer_int

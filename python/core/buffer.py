import ctypes
import symbols.buffer
import core.hook
import cutil

from symbols.vec import VecFreeOnDel

class Buffer:
    def __init__(self, ptr):
        self.struct = ctypes.cast(ptr, symbols.buffer.buffer_p)

        @hooks.delete_struct(50)
        def handle_delete(struct):
            if struct == self.struct:
                self.valid = False

        self.handle_delete = handle_delete

    @property
    def struct(self):
        if not self.valid:
            raise Exception("Invalid buffer")

        return self._struct

    @struct.setter
    def struct(self, value):
        self.valid   = True
        self._struct = value

    def __len__(self):
        return symbols.buffer.len(self.struct)

    def __getitem__(self, index):
        struct = symbols.buffer.get_line(self.struct, index)

        return VecFreeOnDel(struct, ctypes.c_char)

    def __setitem__(self, index, vec):
        if isinstance(vec, (str, bytes)):
            vec = symbols.vec.str2vec(vec)

        symbols.buffer.set_line(self.struct, index, vec.struct)

    def __eq__(self, other):
        return self.struct == other.struct

    def insert(self, index):
        symbols.buffer.insert(self.struct, index)

    def delete(self, index):
        symbols.buffer.delete(self.struct, index)

    def read(self, path):
        fptr = cutil.fptr_open(path, "r")

        symbols.io.file.read_buffer(self.struct, fptr)

        cutil.fptr_close(fptr)

    def push(self, s):
        self.insert(0)
        self[0] = s

class hooks:
    create = core.hook.Hook(
        symbols.buffer.on_create,
        Buffer,
        symbols.buffer.lineno)

    delete = core.hook.Hook(
        symbols.buffer.on_delete,
        Buffer,
        symbols.buffer.lineno)

    delete_struct = core.hook.Hook(
        symbols.buffer.on_delete,
        symbols.buffer.buffer_p,
        symbols.buffer.lineno)

    class line:
        change_pre  = core.hook.Hook(
            symbols.buffer.line.on_change_pre,
            Buffer,
            symbols.buffer.lineno,
            symbols.vec.Vec.Type(ctypes.c_char))

        change_post = core.hook.Hook(
            symbols.buffer.line.on_change_post,
            Buffer,
            symbols.buffer.lineno,
            symbols.vec.Vec.Type(ctypes.c_char))

        delete_pre  = core.hook.Hook(
            symbols.buffer.line.on_delete_pre,
            Buffer,
            symbols.buffer.lineno)

        delete_post = core.hook.Hook(
            symbols.buffer.line.on_delete_post,
            Buffer,
            symbols.buffer.lineno)

        insert_pre = core.hook.Hook(
            symbols.buffer.line.on_insert_pre,
            Buffer,
            symbols.buffer.lineno)

        insert_post = core.hook.Hook(
            symbols.buffer.line.on_insert_post,
            Buffer,
            symbols.buffer.lineno)

import ctypes
import symbols.buffer
import cutil
import weakref

from core.properties import Properties
from core.vec        import VecFreeOnDel, Vec
from core.hook       import Hook

class BufferContainer:
    def __init__(self):
        # This contains reference by pointer (to the struct. Integers
        # for speed) Objects are automatically removed from it when they
        # leave scope. They are not kept alive here.
        self.by_ptr = weakref.WeakValueDictionary()

    def mount(self):
        # Only one of these methods is mounted, in contrast to the method
        # where every BufferObj mounts its own method. (ew)
        #
        # This function stops Segmentation Faults where a buffer is freed
        # and a reference to it still exists in Python. The python reference
        # contains a non-null pointer to freed memory, and if it tries to
        # operate on it, it things explode.
        @hooks.delete_struct(50)
        def handle_delete(struct):
            ptr = cutil.ptr2int(struct)
            b   = self.by_ptr.get(ptr)

            # If we have no idea what just got deleted, return.
            if b == None:
                return

            # Set the object to be non-valid. It will throw exceptions if
            # we try and access its struct.
            b.valid = False
            # We also need to delete /our/ reference to this pointer, since
            # if it gets reallocated, we need to not return an invalid and
            # broken pointer.
            del self.by_ptr[ptr]

        # We also need to keep a reference around to the hook function we made,
        # otherwise it will get GC'd and core/hook.py will delete it also.
        self.handle_delete = handle_delete

    # We need to pretend like we just initialized this, and didn't just
    # find it already alive in a trashcan someplace.
    def __call__(self, struct):
        ptr = cutil.ptr2int(struct)
        b   = self.by_ptr.get(ptr)

        # If we have a BufferObj with the same pointer, return that.
        if b != None:
            return b

        # Otherwise create a new one, and save a reference to it.
        b = BufferObj(struct)
        self.by_ptr[ptr] = b

        return b

    def new(self):
        ptr = symbols.buffer.init()
        return self(ptr)

class BufferObj:
    def __init__(self, ptr):
        self.struct = ctypes.cast(ptr, symbols.buffer.buffer_p)

    @property
    def struct(self):
        if not self.valid:
            raise Exception("Invalid buffer")

        return self._struct

    @struct.setter
    def struct(self, value):
        self.valid   = True
        self._struct = value
        propptr = symbols.buffer.get_properties(value)
        self.properties = Properties(propptr)

    def __len__(self):
        return symbols.buffer.len(self.struct)

    def __getitem__(self, index):
        struct = symbols.buffer.get_line(self.struct, index)

        if cutil.isnull(struct):
            return None

        return VecFreeOnDel(struct, ctypes.c_char)

    def __setitem__(self, index, vec):
        if isinstance(vec, (str, bytes)):
            vec = Vec.from_string(vec)

        symbols.buffer.set_line(self.struct, index, vec.struct)

    def __delitem__(self, index):
        self.delete(index)

    def __eq__(self, other):
        return  isinstance(other, BufferObj) and \
            cutil.ptreq(self.struct, other.struct)

    def __hash__(self):
        return cutil.ptr2int(self.struct)

    def insert(self, index):
        symbols.buffer.insert(self.struct, index)

    def delete(self, index):
        symbols.buffer.delete(self.struct, index)

    def read(self, path):
        fptr = cutil.fptr_open(path, "r")

        symbols.io.file.read_buffer(self.struct, fptr)

        cutil.fptr_close(fptr)

    def dump(self, path):
        fptr = cutil.fptr_open(path, "w")

        symbols.io.file.dump_buffer(self.struct, fptr)

        cutil.fptr_close(fptr)

    def push(self, s):
        self.insert(0)
        self[0] = s

Buffer = BufferContainer()

def initsys():
    Buffer.mount()

class hooks:
    batch_region = Hook(
        symbols.buffer.on_batch_region,
        Buffer,
        symbols.buffer.lineno,
        symbols.buffer.lineno)

    create = Hook(
        symbols.buffer.on_create,
        Buffer)

    delete = Hook(
        symbols.buffer.on_delete,
        Buffer)

    delete_struct = Hook(
        symbols.buffer.on_delete,
        symbols.buffer.buffer_p,
        symbols.buffer.lineno)

    class line:
        change_pre  = Hook(
            symbols.buffer.line.on_change_pre,
            Buffer,
            symbols.buffer.lineno,
            Vec.Type(ctypes.c_char))

        change_post = Hook(
            symbols.buffer.line.on_change_post,
            Buffer,
            symbols.buffer.lineno,
            Vec.Type(ctypes.c_char))

        delete_pre  = Hook(
            symbols.buffer.line.on_delete_pre,
            Buffer,
            symbols.buffer.lineno)

        delete_post = Hook(
            symbols.buffer.line.on_delete_post,
            Buffer,
            symbols.buffer.lineno)

        insert_pre = Hook(
            symbols.buffer.line.on_insert_pre,
            Buffer,
            symbols.buffer.lineno)

        insert_post = Hook(
            symbols.buffer.line.on_insert_post,
            Buffer,
            symbols.buffer.lineno)

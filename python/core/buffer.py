import ctypes
import symbols.buffer
import core.hook
import core.properties
import cutil
import weakref

from symbols.vec import VecFreeOnDel

# An instance of this class acts a little like a class.
# When called with a structure pointer to a buffer, this class
# returns an object already representing that buffer if it exists,
# or creates a new one if it does not. This means that setting or
# modifying BufferObj objects can break things heavily, so isn't
# the best idea.
#
# Note that if someone somewhere starts tracking every instance of
# buffer creation and deletion, and stores them in a list. This will
# be a cache of all buffers.
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
        self.properties = core.properties.Properties(propptr)

    def __len__(self):
        return symbols.buffer.len(self.struct)

    def __getitem__(self, index):
        struct = symbols.buffer.get_line(self.struct, index)

        if cutil.isnull(struct):
            return None

        return VecFreeOnDel(struct, ctypes.c_char)

    def __setitem__(self, index, vec):
        if isinstance(vec, (str, bytes)):
            vec = symbols.vec.str2vec(vec)

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

class hooks:
    batch_region = core.hook.Hook(
        symbols.buffer.on_batch_region,
        Buffer,
        symbols.buffer.lineno,
        symbols.buffer.lineno)

    create = core.hook.Hook(
        symbols.buffer.on_create,
        Buffer)

    delete = core.hook.Hook(
        symbols.buffer.on_delete,
        Buffer)

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

Buffer.mount()

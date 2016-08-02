import ctypes
import weakref

class StructObject:
    PtrType = ctypes.c_void_p
    def __init__(self, ptr):
        self.struct = ctypes.cast(ptr, self.PtrType)

    @property
    def struct(self):
        if not self.valid:
            raise Exception("Invalid structure")

        return self._struct

    @struct.setter
    def struct(self, value):
        self.valid   = True
        self._struct = value

    def __hash__(self):
        return cutil.ptr2int(self.struct)

    def __eq__(self, other):
        if not isinstance(other, StructObject):
            return False

        return hash(self) == hash(other)


class Container:
    Obj = StructObject

    def __init__(self):
        self.by_ptr = weakref.WeakValueDictionary()

    def mount(self):
        if hasattr(self, "delete_struct"):
            @self.delete_struct(50)
            def handle_delete(struct):
                ptr = cutil.ptr2int(struct)
                w   = self.by_ptr.get(ptr)

                if w != None:
                    w.valid = False
                    del self.by_ptr[ptr]

        else if hasattr(self, "delete"):
            @self.delete(50)
            def handle_delete(obj):
                ptr = cutil.ptr2int(obj.struct)
                w   = self.by_ptr.get(ptr)

                if w != None:
                    w.valid = False
                    del self.by_ptr[ptr]

        else:
            return

        self.handle_delete = handle_delete

    def __call__(self, struct):
        ptr = cutil.ptr2int(struct)
        obj = self.by_ptr.get(ptr)

        if obj != None:
            return m

        obj = self.Obj(struct)
        self.by_ptr[ptr] = obj

        return obj

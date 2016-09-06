"""
File containing stuff for constructing and managing struct-based objects.
"""
import ctypes
import weakref

import cutil

class StructObject:
    """A class representing an instance of a struct.

    This is the base class for various struct-based objects, like windows,
    buffers etc.

    The struct in this class can be invalidated, which is done when it is
    deleted. This means that we will not segfault when we try and access it.
    """

    # This is the struct pointer represented here.
    # Subclasses will want to change this.
    PtrType = ctypes.c_void_p

    def __init__(self, ptr):
        """Initialize an instance of this class.

        Arguments:
            ptr (ctypes.POINTER(...)): A pointer to the struct this class
                represents. May be of a different pointer type.
        """
        self.struct = ctypes.cast(ptr, self.PtrType)

    @property
    def struct(self):
        """Gets the struct property of the class.

        If the struct value is not valid, raises an Exception.
        """

        # Ensure that nothing has invalidated our pointer.
        if not self.valid:
            raise Exception("Invalid structure")

        return self._struct

    @struct.setter
    def struct(self, value):
        """Sets the struct property of the class.

        After being set, the struct is valid.

        Arguments:
            value (*): The value to set the struct property of this object to.
                Generally a pointer to a struct.
        """

        # Since this is a new pointer, assume it is valid.
        self.valid   = True
        self._struct = value

    def __hash__(self):
        """Produce a unique integer hash for this particular struct.

        This is a pointer to the struct. Either its unique or you're having a
        very bad day.
        """

        return cutil.ptr2int(self.struct)

    def __eq__(self, other):
        """Check whether this instance represents the same struct as another.

        Arguments:
            other (StructObject): Another StructObject to compare this one to.
        """
        # Check that we share the same class as this object
        if not isinstance(other, type(self)):
            return False

        return hash(self) == hash(other)


class Container:
    """A class to manage all instances of any StructObject type.

    This class keeps track of the pointers of each StructObject, so that instead
    of creating multiple to represent the same class, a single one can be
    reused.

    The Container class also keeps track of what objects are deleted and freed
    by listening to a 'delete' hook, and invalidates the structs of these
    objects to ensure that they do not cause segmentation faults.
    """
    # The type of StructObject represented by this Container
    # Subclasses of Container should redefine this.
    Obj = StructObject

    #  delete_struct = ...
    #  delete        = ...
    #
    # Set one of these variables in subclasses of this class to hooks that are
    # called with a struct before it is deleted. delete_struct should be called
    # with a struct pointer, while delete should be called with a StructObject.

    def __init__(self):
        """Initialize an instance of this class."""

        # Use a weakref dictionary to keep StructObjects in, so that we do not
        # keep them alive when not necessary.

        # For optimization in the future, consider using some other kind of
        # cache for this purpose, with more effective cache invalidation.
        #
        # Oh god that sounds terrifying...
        self.by_ptr = weakref.WeakValueDictionary()

        # Mount deletion handlers to their hooks
        self.mount()

    def mount(self):
        """Mount the deletion handlers of this Container to their hooks."""

        # Check if we have a delete_struct attribute...
        if hasattr(self, "delete_struct"):
            @self.delete_struct(900)
            def handle_delete(struct):
                # Get an int pointer from the struct, and use this to look it
                # up in our dictionary of structs.
                ptr  = cutil.ptr2int(struct)
                inst = self.by_ptr.get(ptr)

                # If it exists, set it invalid, and delete it from our weak
                # dictionary.
                if inst != None:
                    inst.valid = False
                    del self.by_ptr[ptr]

        # Check if we have a delete attribute...
        elif hasattr(self, "delete"):
            @self.delete(900)
            def handle_delete(obj):
                # Get an int pointer to our StructObject by hashing it, use this
                # to look it up in our dictionary of structs.
                ptr  = hash(obj)
                inst = self.by_ptr.get(ptr)

                # If it exists, set it invalid, and delete it from out weak
                # dictionary.
                if inst != None:
                    inst.valid = False
                    del self.by_ptr[ptr]

        # If we have neither, return...
        else:
            return

        # If we defined a handler, keep it as an attribute of this struct, so it
        # won't be GC'd, and so we can manipulate it later if we want.
        self.handle_delete = handle_delete

    def __call__(self, struct, objtype=None):
        """Return a StructObject representing a struct pointer.

        All pointers handed to this function should be very valid to avoid
        implosion. Where possible, returns an already-existing StructObject,
        otherwise creates a new one.

        Arguments:
            struct (ctypes.POINTER(...)): A pointer to the struct that the new
                StructObject should represent. Must be a valid pointer.

        Return (StructObject):
            A StructObject representing the pointer handed to this funtion.
        """

        if objtype == None:
            objtype = self.Obj

        # Get an integer from the pointer, and try and find a matching
        # StructObject in the weak dictionary.
        ptr = cutil.ptr2int(struct)
        obj = self.by_ptr.get(ptr)

        # If a StructObject already exists, return it
        if obj != None:
            return obj

        # Otherwise, create a new one, save it to the weak dictionary,
        # and return it.
        obj = objtype(struct)
        self.by_ptr[ptr] = obj

        return obj

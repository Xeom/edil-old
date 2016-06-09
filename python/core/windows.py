import symbols.win

import core.hook
from core.buffer import Buffer
import ctypes
import cutil
import weakref

class WindowContainer:
    def __init__(self):
        self.by_ptr = weakref.WeakValueDictionary()

    def mount(self):
        @hooks.delete_struct(50)
        def handle_delete(struct):
            ptr = cutil.ptr2int(struct)
            w   = self.by_ptr.get(ptr)

            if w == None:
                return

            w.valid = False
            del self.by_ptr[ptr]

        self.handle_delete = handle_delete

    def __call__(self, struct):
        ptr = cutil.ptr2int(struct)
        w   = self.by_ptr.get(ptr)

        if w != None:
            return w

        w = WindowObj(struct)
        self.by_ptr[ptr] = w

        return w

class direction(symbols.win.dir):
    pass

Window = WindowContainer()

def initsys():
    symbols.win.initsys()
    Window.mount()

def killsys():
    symbols.win.killsys()

def get_selected():
    return Window(symbols.win.select.get())

def get_root():
    return Window(symbols.win.root)

class WindowObj:
    def __init__(self, ptr):
        self.struct = ctypes.cast(ptr, symbols.win.win_p)

    @property
    def struct(self):
        if not self.valid:
            raise Exception("Invalid window")

        return self._struct

    @struct.setter
    def struct(self, value):
        self.valid   = True
        self._struct = value

    @property
    def offsetx(self):
        return symbols.win.get_offsetx(self.struct)

    @offsetx.setter
    def offsetx(self, v):
        symbols.win.set_offsetx(self.struct, v)

    @property
    def offsety(self):
        return symbols.win.get_offsety(self.struct)

    @offsety.setter
    def offsety(self, v):
        symbols.win.set_offsety(self.struct, v)

    @property
    def size(self):
        return (symbols.win.size.get_x(self.struct),
                symbols.win.size.get_y(self.struct))

    @property
    def textsize(self):
        x, y = self.size
        return (x - 1, y - 1)

    @property
    def pos(self):
        return (symbols.win.pos.get_x(self.struct),
                symbols.win.pos.get_y(self.struct))

    @property
    def selected(self):
        return cutil.ptreq(symbols.win.select.get(),
                           self.struct)

    def select(self):
        symbols.win.select.set(self.struct)

    @property
    def caption(self):
        return symbols.win.label.caption_get(self.struct)

    @caption.setter
    def caption(self, cap):
        if isinstance(cap, str):
            cap = cap.encode("ascii")

        symbols.win.label.caption_set(self.struct, cap)

    @property
    def sidebar(self):
        return symbols.win.label.sidebar_get(self.struct)

    @caption.setter
    def sidebar(self, sbar):
        if isinstance(sbar, str):
            sbar = sbar.encode("ascii")

        symbols.win.label.sidebar_set(self.struct, sbar)

    @property
    def buffer(self):
        return Buffer(symbols.win.get_buffer(self.struct))

    @buffer.setter
    def buffer(self, b):
        symbols.win.set_buffer(self.struct, b.struct)

    @property
    def parent(self):
        return Window(symbols.win.get_parent(self.struct))

    def delete(self):
        symbols.win.delete(self.struct)

        self.valid = False

    def split(self, direction):
        symbols.win.split(self.struct, direction)

    def __eq__(self, other):
        if not isinstance(other, WindowObj):
            return False

        return hash(self) == hash(other)

    def __iter__(self):
        sub = symbols.win.iter.first(self.struct)
        last = symbols.win.iter.last(self.struct)

        while not cutil.ptreq(sub, last):
            yield Window(sub)
            sub = symbols.win.iter.next(sub)

        yield Window(sub)


    def __hash__(self):
        return cutil.ptr2int(self.struct)

    def next(self):
        return Window(symbols.win.iter.next(self.struct))

    def prev(self):
        return Window(symbols.win.iter.prev(self.struct))

    def adj(self, n):
        symbols.win.size.adj_splitter(self.struct, n)

    def isleaf(self):
        return bool(symbols.win.type_isleaf(self.struct))

    def issplitter(self):
        return bool(symbols.win.type_issplitter(self.struct))

class hooks:
    class size:
        adj_pre = core.hook.Hook(
            symbols.win.size.on_adj_pre,
            Window,
            ctypes.c_int)

        adj_post = core.hook.Hook(
            symbols.win.size.on_adj_post,
            Window,
            ctypes.c_uint,
            ctypes.c_uint)

    split = core.hook.Hook(
        symbols.win.on_split,
        Window,
        Window)

    delete_post = core.hook.Hook(
        symbols.win.on_delete_post,
        Window)

    delete_pre = core.hook.Hook(
        symbols.win.on_delete_pre,
        Window)

    delete_struct = core.hook.Hook(
        symbols.win.on_delete_pre,
        symbols.win.win_p)

    create = core.hook.Hook(
        symbols.win.on_create,
        Window)

    select = core.hook.Hook(
        symbols.win.on_select,
        Window,
        Window)

    buffer_set = core.hook.Hook(
        symbols.win.on_buffer_set,
        Window,
        Buffer)

    offsetx_set = core.hook.Hook(
        symbols.win.on_offsetx_set,
        Window,
        ctypes.c_ulong)

    offsety_set = core.hook.Hook(
        symbols.win.on_offsety_set,
        Window,
        ctypes.c_ulong)

    class sidebar:
        set_pre = core.hook.Hook(
            symbols.win.label.on_sidebar_set_pre,
            Window,
            ctypes.c_char_p)

        set_post = core.hook.Hook(
            symbols.win.label.on_sidebar_set_post,
            Window,
            ctypes.c_char_p)

    class caption:
        set_pre = core.hook.Hook(
            symbols.win.label.on_caption_set_pre,
            Window,
            ctypes.c_char_p)

        set_post = core.hook.Hook(
            symbols.win.label.on_caption_set_post,
            Window,
            ctypes.c_char_p)

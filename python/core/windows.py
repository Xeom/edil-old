import signal

import symbols.win

import core.hook

from core.buffer    import Buffer
from core.container import Container, StructObject

import ctypes
import cutil
import weakref

class direction(symbols.win.dir):
    pass

class WindowObj(StructObject):
    """A class to contain and represent a window.

    Windows are used to display the contents of a buffer in edil. Their sizes,
    positions, etc. are managed by the C window system.

    Since windows are managed by the window system, and so we do not need to
    worry about orphan windows, or keeping a reference to all windows in python
    ay all times.
    """

    PtrType = symbols.win.win_p

    @property
    def offsetx(self):
        """Return the current number of columns of offset of the window.

        Offset is how many characters a window is moved relative to its
        contents. i.e. How far it has been scrolled.

        Return (int): The x offset of this window.
        """

        return symbols.win.get_offsetx(self.struct)

    @offsetx.setter
    def offsetx(self, v):
        """Set the current number of columns of offset of the window.

        Offset is how many characters a window is moved relative to its
        contents. i.e. How far it has been scrolled.

        Arguments:
            v (int): The new value for the x offset of the window.
        """

        symbols.win.set_offsetx(self.struct, v)

    @property
    def offsety(self):
        """Return the current number of lines of offset of the window.

        Offset is how many characters a window is moved relative to its
        contents. i.e. How far it has been scrolled.

        Return (int): The y offset of this window.
        """

        return symbols.win.get_offsety(self.struct)

    @offsety.setter
    def offsety(self, v):
        """Set the current number of lines of offset of the window.

        Offset is how many characters a window is moved relative to its
        contents. i.e. How far it has been scrolled.

        Arguments:
            v (int): The new value for the y offset of the window.
        """

        symbols.win.set_offsety(self.struct, v)

    @property
    def size(self):
        """Get the current size of this window.

        Return ( (int, int) ): A tuple of two integers, the x and y size of this
            window in numbers of characters.
        """

        return (symbols.win.size.get_x(self.struct),
                symbols.win.size.get_y(self.struct))

    @property
    def textsize(self):
        """Get the current size of buffer that can be displayed by this window.

        Simply subtracts area for the sidebars from .size().

        Return ( (int, int) ): A tuple of two integers, the x and y size of the
            area of this window that can display buffer.
        """

        x, y = self.size

        return (x - 1, y - 1)

    @property
    def pos(self):
        """Get the current ncurses coordinates of this window.

        The coordinates are given for the top-left corner of the window. The
        origin is the top-left corner of the screen.

        Return ( (int, int) ): A tuple of two integers ( x and y ) representing
            the position of this window.
        """

        return (symbols.win.pos.get_x(self.struct),
                symbols.win.pos.get_y(self.struct))

    @property
    def selected(self):
        """Test whether this window is currently selected.

        Return (bool): True if this window is currently selected, False 
            otherwise.
        """

        return cutil.ptreq(symbols.win.select.get(),
                           self.struct)

    def select(self):
        """Make this window the currently selected window."""

        symbols.win.select.set(self.struct)

    @property
    def caption(self):
        """Gets the current caption of this window.

        The caption is a line of text displayed on the lower border of this
        window.

        Return (bytes): The current caption.
        """

        return symbols.win.label.caption_get(self.struct)

    @caption.setter
    def caption(self, cap):
        """Sets the current caption of this window.

        The caption is a line of text displayed on the lower border of this
        window.

        Arguments:
            cap (bytes, str): The new value of the caption of this window.
        """

        if isinstance(cap, str):
            cap = cap.encode("ascii")

        symbols.win.label.caption_set(self.struct, cap)

    @property
    def sidebar(self):
       """Gets the current sidebar of this window.

       The sidebar is a column of text displayed on the right border of this
       window.

       Return (bytes): The current sidebar.
       """

       return symbols.win.label.sidebar_get(self.struct)

    @caption.setter
    def sidebar(self, sbar):
        """Sets the current sidebar of this window.

        The sidebar is a column of text displayed on the right border of this
        window.

         Arguments:
            sbar (bytes, str): The new value of the sidebar of this window.
        """

        if isinstance(sbar, str):
            sbar = sbar.encode("ascii")

        symbols.win.label.sidebar_set(self.struct, sbar)

    @property
    def buffer(self):
        """Get the current buffer displayed in this window.

        Return (Buffer): The current buffer, None if there is no current buffer.
        """

        return Buffer(symbols.win.get_buffer(self.struct))

    @buffer.setter
    def buffer(self, b):
        """Set the buffer displayed in this window.

        Arguments:
            b (Buffer): The new buffer to display in this window.
        """

        symbols.win.set_buffer(self.struct, b.struct)

    @property
    def parent(self):
        """Get the parent of the currently selected window.

        All non-root windows have a parent, in the form of a splitter, which
        divides space between this window and its sibling. The windows are
        represented in this way as a bitree.

        Return (Window): The parent of this window.
        """

        return Window(symbols.win.get_parent(self.struct))

    def delete(self):
        """Delete this window.

        This window will become invalid and unusable after doing this. A
        splitter window will also be deleted as its position is filled by this
        window's sibling.
        """

        symbols.win.delete(self.struct)

        self.valid = False

    def split(self, direction):
        """Split this window, creating a new window.

        Creates a splitter in the space currently occupied by this window,
        containing both this window and a new leaf window.

        Arguments:
            direction (str): One of "up", "down", "left", "right". The position,
                relative to this window, that we want the new leaf in.
        """

        return Window(symbols.win.split(self.struct, direction))

    def __iter__(self):
        """Produce an iterator accross this window and all its sub-windows.

        The windows are iterated accross from top-left to bottom-right.

        Return (iter): An iterator of the Windows.
        """

        sub = symbols.win.iter.first(self.struct)
        last = symbols.win.iter.last(self.struct)

        while not cutil.ptreq(sub, last):
            yield Window(sub)
            sub = symbols.win.iter.next(sub)

        yield Window(sub)

    def next(self):
        """Find the 'next' window after this one.

        Iterates through the tree of windows in sequence to find a next window.
        Loops once it reaches the end of windows.

        Return (Window): The next window.
        """

        return Window(symbols.win.iter.next(self.struct))

    def prev(self):
        """Find the 'previous' window before this one.

        This function is the inverse of .next()

        Return (Window): The previous window.
        """

        return Window(symbols.win.iter.prev(self.struct))

    def adj(self, n):
        """Adjust the position of the split if this is a splitter.

        Moves the split in a splitter window n characters right/down (depending
        on the orientation of the splitter). Negative values move the split up
        or left.

        Arguments:
            n (int): The number of characters to move the splitter.
        """

        symbols.win.size.adj_splitter(self.struct, n)

    def isleaf(self):
        """Check if this window is a leaf (has a buffer, but no children)

        Return (bool): True if this window is a leaf, False otherwise.
        """

        return bool(symbols.win.type_isleaf(self.struct))

    def issplitter(self):
        """Check if this window is a splitter (has children, but no buffer)

        Return (bool): True if this window is a splitter, False otherwise.
        """

        return bool(symbols.win.type_issplitter(self.struct))

class WindowContainer(Container):
    Obj           = WindowObj
    delete_struct = core.hook.Hook(symbols.win.on_delete_pre,
                                   symbols.win.win_p)


Window = WindowContainer()

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

def initsys():
    symbols.win.initsys()
    Window.mount()

def killsys():
    symbols.win.killsys()

def get_selected():
    return Window(symbols.win.select.get())

def get_root():
    return Window(symbols.win.root)

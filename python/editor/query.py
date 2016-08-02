from core.key import Key
from core.face import Face
import core.keymap
import core.ui
import editor.autocomplete

from core.cursor import CursorType
import core.cursor

import os

query_prefix_face = Face(Face.black, Face.cyan)

class AutoCompleter:
    """Class to represent the state of autocompletion of a query.

    Arguments:
        callback (funct): Called with a semi-competed string, that returns an
                          iterable of the various possible completions of that
                          string.
    """

    def __init__(self, callback):
        # Current availible options to autocomplete to
        self.options = []
        # The next index in the list of options autocompleted to
        self.index   = 0
        # The last string the query's contents were set to
        self.last    = None
        # A callback to get all current autocomplete options
        self.cb      = callback

    def nextoption(self):
        """Get the next autocomplete option to be suggested"""

        # None -> There are no valid autocompletes
        if not self.options:
            return None

        # Return the next string from the list of options
        rtn = self.options[self.index]

        # Increment the index so we return the next one next time
        self.index += 1
        self.index %= len(self.options)

        return rtn

    def complete(self, semi):
        """Suggest an autocomplete option for a semi-completed string.

        If called repeatedly for the same string, this function returns
        different suggestions on each subsequent call.

        The first suggestion, if relevant, will be a common prefix of all
        possible full completions.

        Arguments:
            semi (str): The semi-completed string to suggest a completion of.
        """

        if self.cb == None:
            return None

        # If something other than us has changed the query contents,
        # generate a new set of options.
        if self.last != semi:
            self.options = list(self.cb(semi))
            self.index   = 0

            # Add another option - The common prefix of all options.
            # This should be the default if the user presses autocompletes
            # only once, so we insert it at the front of the list.
            #
            # I'm a lil scared that os.path.commonprefix is somehow gonna break
            # if I throw /es at it or something, but meh.
            if len(self.options) > 1:
                self.options.insert(0, os.path.commonprefix(self.options))

        # Get the next option and remember what it is
        rtn = self.nextoption()
        self.last = rtn

        return rtn

class QueryCursor:
    """A cursor to enter query responses with.

    The methodnames here are the ones used by CursorType."""

    def __init__(self):
        self.cn = 0
        # The current contents of the response, as a list of single characters.
        self.string = []
        # We use the state the statusbar already has as our prefix.
        self.prefix = core.ui.get_sbar()
        self.draw()

    # This init function is called with the buffer of the cursor.
    # It is the function used by the C cursor system.
    def init(self, b):
        self.buffer = b

    def get_buffer(self):
        return self.buffer

    def get_cn(self):
        return self.cn

    def draw(self):
        string = bytearray("".join(self.string), "ascii")
        # Insert the special cursor escape code in the string.
        string[self.cn:self.cn] = core.ui.cursorface

        core.ui.set_sbar(self.prefix + bytes(string))

    def move_cols(self, n):
        self.cn += n
        # Limit the cursor's position to be in the string.
        self.cn  = min(len(self.string), self.cn)
        self.cn  = max(0,                self.cn)

        # Redraw with the cursor in the correct place.
        self.draw()

    def delete(self, n):
        del self.string[max(0, self.cn - n):self.cn]

        # Reposition cursor.
        self.cn -= n
        self.cn  = max(0, self.cn)

        # Redraw
        self.draw()

    def insert(self, string):
        # Accept bytes or strings.
        if isinstance(string, bytes):
            string = string.decode("ascii")

        # Insert and reposition cursor.
        self.string[self.cn:self.cn] = list(string)
        self.cn += len(string)

        self.draw()

    # Not a cursor function :D Used for autocompleting.
    def set(self, string):
        """Set the contents of the query response to something.

        The cursor is positioned at the end of the new response.

        Arguments:
            string (str, bytes): The new contents of the response.
        """
        if isinstance(string, bytes):
            string = string.decode("ascii")

        self.string = list(string)
        self.cn     = len(string)

        self.draw()

QueryCursorType = CursorType(QueryCursor)

core.cursor.snap_blacklist.insert(0, QueryCursorType.struct)
#cursor.Snapper.blacklist.append(QueryPoint)

mapname = "query-default"

core.keymap.maps.add(mapname)
querymap = core.keymap.maps[mapname]

querying_buffers = set()

def autocomplete_query():
    curcursor = core.cursor.get_selected()
    curinst   = QueryCursorType.find_instance(curcursor)

    if not isinstance(curinst, QueryCursor):
        raise Exception("Not in a query")

    auto = curinst.autocompleter.complete("".join(curinst.string))

    if auto != None:
        curinst.set(auto)

def make_query(callback, prefix=b"", completecallback=None):
    core.keymap.frames.active.push("query-default")

    buf  = core.windows.get_selected().buffer

    if buf in querying_buffers:
        raise Exception("Cannot double-query")

    if isinstance(prefix, str):
        prefix = prefix.encode("ascii")

    core.ui.set_sbar(query_prefix_face.serialize(len(prefix)) + prefix)

    querying_buffers.add(buf)

    newcursor = core.cursor.spawn(buf, QueryCursorType)
    core.cursor.select_last(buf)
    newinst   = QueryCursorType.find_instance(newcursor)
    newinst.autocompleter = AutoCompleter(completecallback)
    newinst.callback      = callback

def confirm(callback, message=None):
    if message == None:
        message = b"Are you sure?"

    def cb(string):
        string = string.lower()

        if string == "yes":
            callback()

        if string == "no":
            return

        else:
            confirm(callback, b"Please type yes or no")

    make_query(cb, message + " ", editor.autocomplete.options("yes", "no"))

def leave_query():
    curcursor = core.cursor.get_selected()
    curinst   = QueryCursorType.find_instance(curcursor)

    buf = core.windows.get_selected().buffer

    core.keymap.frames.active.remove("query-default")

    if buf in querying_buffers:
        querying_buffers.remove(buf)

    core.ui.set_sbar(b"")
    core.cursor.delete_selected(buf)
    curinst.callback("".join(curinst.string))

@querymap.add(Key("RETURN"))
def query_enter(keys):
    leave_query()

@querymap.add(Key("TAB"))
def query_tab(keys):
    autocomplete_query()


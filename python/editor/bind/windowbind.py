import core.windows

import editor.buffers.userlog

from core.mode import Mode
from core.key  import Key

from editor.autocomplete import options
from editor.command import Command, CommandArg

mode = Mode.new(100, "default-window")
kmap = mode.keymap

modifier_key = Key("W", con=True)

default_cursor_type = core.cursor.types.region

# window-split
#
#
# Splits the currently selected window into two sub-windows. The space occupied
# by the original window will be occupied by two windows - The original one,
# shrunk to fit, and a new window. The arrangement of these two windows is
# determined by the direction argument handed to this command e.g.
#
#      right          left         up      down
# +------+-----+ +-----+------+ +------+ +------+
# |      |     | |     |      | | new  | | orig |
# | orig | new | | new | orig | +------+ +------+
# |      |     | |     |      | | orig | | new  |
# +------+-----+ +-----+------+ +------+ +------+
#
# The command also spawns a new cursor in the new window, of type
# default_cursor_type. (defined in this file.)
split_cmd = Command("window-split",
                    CommandArg(str, "Direction", options(
                        "up", "down", "left", "right")))

@split_cmd.hook(500)
def split_cb(direction):
    direction = direction.lower()

    if direction not in ("up", "down", "left", "right"):
        return

    new = core.windows.get_selected().split(
        {"up":   core.windows.direction.up,
         "down": core.windows.direction.down,
         "left": core.windows.direction.left,
         "right":core.windows.direction.right}[direction])

    core.cursor.spawn(new.buffer, core.cursor.types.region)

    editor.buffers.userlog.log("Split current window..")

split_cmd.map_to(kmap, modifier_key, Key("UP"),    defaultargs=["up"])
split_cmd.map_to(kmap, modifier_key, Key("DOWN"),  defaultargs=["down"])
split_cmd.map_to(kmap, modifier_key, Key("LEFT"),  defaultargs=["left"])
split_cmd.map_to(kmap, modifier_key, Key("RIGHT"), defaultargs=["right"])

# window-delete
#
# Deletes the currently selected window. The space occupied by the window is
# taken up by enlarging the other window in its splitter. The root window cannot
# be deleted.
del_cmd = Command("window-delete")

@del_cmd.hook(500)
def del_cb():
    core.windows.get_selected().delete()
    editor.buffers.userlog.log("Deleted selected window")

del_cmd.map_to(kmap, modifier_key, Key("k"))

# window-next
#
# Select the n-th next window. After iterating over all windows, this command
# returns to the first window. Negative values select previous windows.
next_cmd = Command("window-next",
                   CommandArg(int, "# windows to move"))

@next_cmd.hook(500)
def next_cb(n):
    w = core.windows.get_selected()

    if n < 0:
        for i in range(n):
            w = w.prev()

    elif n > 0:
        for i in range(n):
            w = w.next()

    w.select()

next_cmd.map_to(kmap, modifier_key, Key("."), defaultargs=[ 1])
next_cmd.map_to(kmap, modifier_key, Key(","), defaultargs=[-1])

# window-up
#
# Select the window that's the parent of the currently selected window.
up_cmd = Command("window-up",
                 CommandArg(int, "# levels to move up"))

@up_cmd.hook(500)
def up_cb(n):
    w = core.windows.get_selected()

    for i in range(n):
        w = w.parent

    w.select()

up_cmd.map_to(kmap, modifier_key, Key("l"), defaultargs=[1])

# window-adjust
#
# Adjust the boundary between the two sub-windows of a selected splitter. The
# splitter is moved the specified number of spaces either right or down,
# depending on which way it is oriented. Negative values move the splitter up or
# left.
adj_cmd = Command("window-adjust",
                  CommandArg(int, "Number of spaces"))

@adj_cmd.hook(500)
def adj_cb(n):
    core.windows.get_selected().adj(n)

adj_cmd.map_to(kmap, modifier_key, Key("+"), defaultargs=[ 1])
adj_cmd.map_to(kmap, modifier_key, Key("-"), defaultargs=[-1])

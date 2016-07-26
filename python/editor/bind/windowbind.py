import core.windows
import core.keymap

import editor.buffers.userlog

from core.key import Key

from editor.autocomplete import options
from editor.command import Command, CommandArg



core.keymap.maps.add("window-default")
winmap = core.keymap.maps["window-default"]

modifier_key = Key("W", con=True)

# window-split
#
# Split the selected window, replacing the space it currently occupies with two
# new windows. The new window is above, below, to the left or right of the
# selected window, depending on the argument given to the command.

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

@winmap.add(modifier_key, Key("UP"))
def splitup_mapped(keys):
    split_cmd.run_withargs("up")

@winmap.add(modifier_key, Key("DOWN"))
def splitdown_mapped(keys):
    split_cmd.run_withargs("down")

@winmap.add(modifier_key, Key("LEFT"))
def splitleft_mapped(keys):
    split_cmd.run_withargs("left")

@winmap.add(modifier_key, Key("RIGHT"))
def splitright_mapped(keys):
    split_cmd.run_withargs("right")

del_cmd = Command("window-delete")

@del_cmd.hook(500)
def del_cb():
    core.windows.get_selected().delete()
    editor.buffers.userlog.log("Deleted selected window")

@winmap.add(modifier_key, Key("k"))
def del_mapped(keys):
    del_cmd.run()

next_cmd = Command("window-next")

@next_cmd.hook(500)
def next_cb():
    core.windows.get_selected().next().select()

@winmap.add(modifier_key, Key("."))
def next_mapped(keys):
    next_cmd.run()

prev_cmd = Command("window-prev")

@prev_cmd.hook(500)
def prev_cb():
    core.windows.get_selected().prev().select()

@winmap.add(modifier_key, Key("."))
def prev_mapped(keys):
    prev_cmd.run()

up_cmd = Command("window-up")

@up_cmd.hook(500)
def up_cb():
    core.windows.get_selected().parent.select()

@winmap.add(modifier_key, Key("l"))
def up_mapped(keys):
    core.windows.get_selected().parent.select()

adj_cmd = Command("window-adjust",
                  CommandArg(int, "Number of spaces"))

@adj_cmd.hook(500)
def adj_cb(n):
    core.windows.get_selected().adj(n)

@winmap.add(modifier_key, Key("+"))
def adjplus_mapped(keys):
    adj_cmd.run_withargs(1)

@winmap.add(modifier_key, Key("-"))
def adjminus_mapped(keys):
    adj_cmd.run_withargs(-1)

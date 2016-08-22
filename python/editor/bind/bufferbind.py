import core.keymap
import core.windows

from core.key import Key

import editor.buffers.ring
from editor.command import Command, CommandArg

mapname = "buffer-default"

core.keymap.maps.add(mapname)
bufmap = core.keymap.maps[mapname]

modifier_key = Key("B", con=True)

# buffer-select
#
# Select the n-th next buffer. Negative values select previous buffers.
next_cmd = Command("buffer-next",
                   CommandArg(int, "# buffers to move"))

@next_cmd.hook(500)
def next_cb(n):
    w   = core.windows.get_selected().buffer
    buf = editor.buffers.ring.get_id(w, increment=n)
    w.buffer = buf

next_cmd.map_to(bufmap, modifier_key, Key("."), defaultargs=[ 1])
next_cmd.map_to(bufmap, modifier_key, Key(","), defaultargs=[-1])


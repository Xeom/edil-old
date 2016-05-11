import core.keymap

from core.key import Key

import editor.buffers.ring
import core.windows

mapname = "buffer-default"

core.keymap.maps.add(mapname)
curmap = core.keymap.maps[mapname]

@curmap.add(Key("B", con=True), Key("RIGHT"))
def cur_next(keys):
    editor.buffers.ring.nextify_window(core.windows.get_selected())


@curmap.add(Key("B", con=True), Key("LEFT"))
def cur_prev(keys):
    editor.buffers.ring.prevify_window(core.windows.get_selected())

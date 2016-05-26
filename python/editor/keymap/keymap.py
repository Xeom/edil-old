import editor.keymap.cursormap
import editor.keymap.windowmap
import editor.keymap.buffermap

import core.keymap
import core.windows

default_buffer_maps = ["master", "cursor-default", "window-default", "buffer-default"]

core.keymap.maps.add("master")

@core.windows.hooks.select(200)
def handle_select(oldwin, newwin):
    buf = newwin.buffer

    fname = "buffer-{}".format(buf.__hash__())

    if fname in core.keymap.frames:
        return

    core.keymap.frames.add(fname)
    map = core.keymap.frames[fname]

    for default in default_buffer_maps:
        map.push(default)

    core.keymap.frames.switch(fname)

handle_select(None, core.windows.get_selected())

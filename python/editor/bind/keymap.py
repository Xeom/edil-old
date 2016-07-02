import editor.bind.cursorbind
import editor.bind.windowbind
import editor.bind.bufferbind
import editor.bind.filesbind
import editor.bind.tabsbind
import editor.bind.commandbind

import core.keymap
import core.windows

default_buffer_maps = ["master",
                       "cmd-default",
                       "cursor-default",
                       "window-default",
                       "buffer-default",
                       "files-default",
                       "tabs-default"]

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

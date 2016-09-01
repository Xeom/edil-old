import editor.bind.cursorbind
import editor.bind.windowbind
import editor.bind.bufferbind
import editor.bind.filesbind
import editor.bind.tabsbind
import editor.bind.commandbind

default_buffer_modes = [editor.bind.cursorbind.mode,
                        editor.bind.windowbind.mode,
                        editor.bind.bufferbind.mode,
                        editor.bind.filesbind.mode,
                        editor.bind.tabsbind.mode,
                        editor.bind.commandbind.mode]

for mode in default_buffer_modes:
    mode.activate()

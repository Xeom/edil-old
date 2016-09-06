import core.windows

from core.mode      import Mode
from core.key       import Key
from editor.command import Command, CommandArg

import editor.autocomplete

mode = Mode.new(100, "default-files")
kmap = mode.keymap

modifier_key = Key("F", con=True)

# file-revert
#
# Load or reload the file associated with the currently selected buffer from
# disk.
revert_cmd = Command("file-revert")

@revert_cmd.hook(500)
def revert_cb():
    editor.files.revert(
        core.windows.get_selected().buffer)

revert_cmd.map_to(kmap, modifier_key, Key("r"))

# file-associate
#
# Associate a particular file path with the currently selected buffer.
associate_cmd = Command("file-associate",
                        CommandArg(str, "File to associate",
                                   editor.autocomplete.path(".")))
@associate_cmd.hook(500)
def associate_cb(path):
    editor.files.associate(
        core.windows.get_selected().buffer, path)

associate_cmd.map_to(kmap, modifier_key, Key("a"))

# file-dump
#
# Write the contents of the currently selected buffer to the file it is
# associated with.
dump_cmd = Command("file-dump")

@dump_cmd.hook(500)
def dump_cb():
    editor.files.dump(
        core.windows.get_selected().buffer)

dump_cmd.map_to(kmap, modifier_key, Key("d"))

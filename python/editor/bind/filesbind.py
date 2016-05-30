import core.windows
import core.keymap

from core.key import Key
from editor.command import Command, CommandArg
import editor.autocomplete

mapname = "files-default"

core.keymap.maps.add(mapname)
filesmap = core.keymap.maps[mapname]

modifier_key = Key("F", con=True)

revert = Command("revert-buffer")

@filesmap.add(modifier_key, Key("r"))
def filesrevert(keys):
    revert.run()

@revert.hook(500)
def revert_cb():
    editor.files.revert(
        core.windows.get_selected().buffer)

associate = Command("associate-buffer",
                    CommandArg(str, "File to associate",
                               editor.autocomplete.path(".")))

@filesmap.add(modifier_key, Key("a"))
def filesassociate(keys):
    associate.run()

@associate.hook(500)
def associate_cb(path):
    editor.files.associate(
        core.windows.get_selected().buffer, path)

dump = Command("dump-buffer")
@filesmap.add(modifier_key, Key("d"))
def filesdump(keys):
    dump.run()

@dump.hook(500)
def dump_cb():
    editor.files.dump(
        core.windows.get_selected().buffer)

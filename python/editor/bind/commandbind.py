import core.keymap
import core.cursor
import core.deferline

from core.key import Key

from editor.autocomplete import options_list
from editor.command import Command, CommandArg, get_command

mapname = "cmd-default"

core.keymap.maps.add(mapname)
cmdmap = core.keymap.maps[mapname]

run_cmd = Command("command-run",
                  CommandArg(str, "Name", options_list(Command.names)))

@run_cmd.hook(500)
def run_cb(name):
    cmd = get_command(name)
    cmd.run()

@cmdmap.add(Key("E", con=True))
def run_mapped(keys):
    run_cmd.run()



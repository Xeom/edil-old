import core.keymap
import core.cursor
import core.deferline

from core.key import Key

from editor.autocomplete import options_list, number
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

repeat_cmd = Command("command-repeat",
                     CommandArg(str, "Name", options_list(Command.names)),
                     CommandArg(int, "Repeats", number()))

proxycmd = Command(None)

repeat_cmds = []
repeat_cbs  = []

@repeat_cmd.hook(500)
def repeat_cb(name, n):
    cmd = get_command(name)

    proxycmd = Command(None, *cmd.args)

    @proxycmd.hook(500)
    def proxycb(*args):
        for i in range(n):
            cmd.run_withargs(*args)

#        repeat_cmds.remove(proxycmd)
 #       repeat_cbs.remove(proxycb)

  #  repeat_cmds.append(proxycmd)
   # repeat_cbs.append(proxycb)

    proxycmd.run()

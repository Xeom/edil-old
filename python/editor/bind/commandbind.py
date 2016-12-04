import shlex

import core.cursor
import core.deferline

from core.mode import Mode
from core.key  import Key

from editor.autocomplete import options_list, number
from editor.command import Command, CommandArg, get_command

mode = Mode.new(100, "default-command")
kmap = mode.keymap

# command-run
#
# Runs a command of a specific name, with no default arguments.

run_cmd = Command("command-run",
                  CommandArg(str, "Name", options_list(Command.names)))

@run_cmd.hook(500)
def run_cb(name):
    cmd = get_command(name)
    cmd.run()

run_cmd.map_to(kmap, Key("E", con=True), Key("x"))

# command-run-line
#
# Runs the command on the current line.

run_line_cmd = Command("command-run-line")

@run_line_cmd.hook(500)
def run_line_cb():
    cur    = core.cursor.get_selected()
    line   = cur.buffer[cur.ln]
    string = bytes(line).decode("ascii")
    parts  = shlex.split(string)
    cmdname = parts[0]
    args    = parts[1:]

    cmd = get_command(cmdname)
    cmd.run_withargs_string(*args)

run_line_cmd.map_to(kmap, Key("E", con=True), Key("l"))

# command-undefault
#
# Increments the undefault counter. For each command argument that would
# normally assume a default value, if the undefault counter is nonzero, the user
# is prompted for a value in place of the default. The counter is then
# decremented.
#
# The last arguments of each command are undefaulted first.

undefault_cmd = Command("command-undefault")

@undefault_cmd.hook(500)
def undefault_cb():
    Command.undefault()

undefault_cmd.map_to(kmap, Key("e", esc=True))

# command-repeat
#
# Repeat a command several times. A specified command is run n times. The user
# is prompted for arguments of the command only once.

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

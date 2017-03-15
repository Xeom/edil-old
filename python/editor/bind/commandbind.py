import shlex

import core.cursor
import core.deferline

from core.mode import Mode
from core.key  import Key

from editor.autocomplete import options_list, number
from editor.command import Command, CommandArg, get_command, hooks

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
    string = bytes(line).decode("utf-8")
    parts  = shlex.split(string)
    cmdname = parts[0]
    args    = parts[1:]

    cmd = get_command(cmdname)
    cmd.run_withargs_string(*args)

run_line_cmd.map_to(kmap, Key("E", con=True), Key("l"))

# command-undefault
#
# Replaces the next default arguments with a specified arguments. The command
# replaces multiple default arguments seperated by spaces. Arguments with spaces
# can be used by enclosing them in quotes. Backslashes can be used to escape
# quotes.
#
# The last arguments of each command are undefaulted first.

undefault_cmd = Command("command-undefault",
                        CommandArg(str, "Substitutes"))

@undefault_cmd.hook(500)
def undefault_cb(subs):
    Command.set_undefaulted(shlex.split(subs))

undefault_cmd.map_to(kmap, Key("E", con=True), Key("u"))

# command-repeat
#
# Repeat a command several times. The next command after the command-repeat
# command is repeated with the same arguments, n times.

repeat_cmd = Command("command-repeat",
                     CommandArg(int, "Repeats", number()))

repeat_cbs = set()

@repeat_cmd.hook(500)
def repeat_cb(n):
    cbenabled = [1]

    @hooks.command_call(500)
    def repcb(name, args):
        if not cbenabled:
            return

        cbenabled.pop()
        repeat_cbs.remove(repcb)

        cmd = get_command(name)

        for i in range(1, n):
            cmd.run_withargs(*args)

    repeat_cbs.add(repcb)

repeat_cmd.map_to(kmap, Key("E", con=True), Key("r"))

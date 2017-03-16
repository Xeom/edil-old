from core.hook import NativeHook
import editor.query

import weakref

class hooks:
    command_call   = NativeHook()
    command_return = NativeHook()

class CommandArg:
    def __init__(self, type, prefix="", completecallback=None):
        if prefix:
            self.prefix  = prefix + ": "
        else:
            prefix = ""

        self.complcb = completecallback
        self.type    = type

    def make_query(self, cb):
        editor.query.make_query(cb, self.prefix, self.complcb)

    def type_convert(self, string):
        return self.type(string)

class Command:
    by_name    = weakref.WeakValueDictionary()
    names      = set()
    undefaulted = []

    def __init__(self, name, *args):
        if name in self.names:
            self.name = None
            raise Exception("This command already exists")

        self.maps = []
        self.name = name
        self.args = args
        self.hook = NativeHook()

        if name != None:
            self.names.add(name)
            self.by_name[name] = self

    @classmethod
    def set_undefaulted(cls, undefaulted):
        cls.undefaulted = undefaulted[:]
        editor.buffers.userlog.log(
            "Set next ({}) default arguments.".format(len(undefaulted)))

    def __del__(self):
        if self.name != None:
            self.names.remove(self.name)

    def get_default_args(self, default):
        if default == None:
            return []

        rtn = default[:]

        for i, arg in enumerate(self.args):
            if not self.undefaulted:
                break

            subst = self.undefaulted.pop(0)

            try:
                rtn[i] = arg.type_convert(subst)
            except:
                editor.buffers.userlog.log(
                    "Substitute argument '{}' is invalid".format(subst))
                self.undefaulted = []
                return default

        return rtn

    def get_arg(self, args):
        n = len(args)

        if n >= len(self.args):
            self.run_withargs(*args)

        else:
            arg = self.args[n]

            def cb(string):
                args.append(arg.type_convert(string))
                self.get_arg(args)

            arg.make_query(cb)

    def run_withargs(self, *args):
        hooks.command_call.call([self.name, args])
        self.hook.call(args)
        hooks.command_return.call([self.name, args])

    def run(self, keys=None, default=None):
        args = self.get_default_args(default)
        self.get_arg(args)

    def map_to(self, keymap, *keys, defaultargs=None):
        def mapped(keys):
            self.run(default=defaultargs)

        self.maps.append(mapped)
        keymap.add(*keys)(mapped)

    def run_withargs_string(self, *strargs):
        self.run_withargs(
            *(a.type_convert(s) for a, s in zip(self.args, strargs))
        )

def get_command(name):
    return Command.by_name[name]


from core.hook import NativeHook
import editor.query

import weakref

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
    query_mode = 0

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
    def query_mode_enable(cls):
        cls.query_mode += 1
        editor.buffers.userlog.log(
            "Enabled query command mode (%s)" % cls.query_mode)

    @classmethod
    def query_mode_disable(cls):
        cls.query_mode = max(cls.query_mode - 1, 0)

    def __del__(self):
        if self.name != None:
            self.names.remove(self.name)

    def get_arg(self, args, n):
        if n >= len(self.args):
            self.hook.call(args)

        else:
            arg = self.args[n]

            def cb(string):
                args.append(arg.type_convert(string))
                self.get_arg(args, n + 1)

            arg.make_query(cb)

    def run_query(self):
        self.get_arg([], 0)

    def run_withargs(self, *args):
        self.hook.call(args)

    def run(self, keys=None, default=None):
        if default == None:
            self.run_query()

        else:
            if self.query_mode and not editor.query.in_query():
                self.run_query()
                type(self).query_mode_disable()

            else:
                self.run_withargs(*default)

    def map_to(self, keymap, *keys, defaultargs=None):
        def mapped(keys):
            self.run(default=defaultargs)

        self.maps.append(mapped)
        keymap.add(*keys)(mapped)

def get_command(name):
    return Command.by_name[name]


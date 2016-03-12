commands = {}


class Command:
    def __init__(self, name, *args):
        commands[name] = self
        self.name = name
        self.args = args

    def __call__(self, f):
        self.f = f

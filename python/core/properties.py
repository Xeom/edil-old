import core.table

class Properties:
    def __init__(self, ptr):
        self.table = core.table.CharTable(ptr)

    def __setitem__(self, name, value):
        value = str(value)
        self.table[name] = value

    def __getitem__(self, name):
        return self.table[name]

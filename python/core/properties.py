import core.table

class Properties:
    def __init__(self, ptr):
        self.table = core.table.CTable(ptr)

    def __setitem__(self, name, value):
        self.table[name] = value

    def __getitem__(self, name):
        return self.table[name]

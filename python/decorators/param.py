import systems.ui

class Choice:
    def __init__(self, prompt, *options):
        self.options = options
        self.prompt  = prompt

    def getvalue(self):
        while True:
            val = systems.ui.prompt(self.prompt + str(tuple(self.options)))
            if val in self.options:
                return val

class Int:
    def __init__(self, prompt):
        self.prompt = prompt

    def getvalue(self):
        while True:
            val = systems.ui.prompt(

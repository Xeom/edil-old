import core.windows

class SubCaption:
    subcaptions = []

    @classmethod
    def update(cls, w):
        for sub in w:
            cls.updatewin(sub)

    @classmethod
    def updatewin(cls, w):
        cap = []
        import sys

        for instance in cls.subcaptions:
            try:
                sub = instance.funct(w)
            except:
                continue

            cap.append(sub)

        w.caption = "-".join(cap)

    def __init__(self, funct):
        self.subcaptions.append(self)
        self.funct   = funct
        self.enabled = False

    def move_left(self):
        ind = self.subcaptions.find(self)
        self.subcaptions.remove(self)
        self.subcaptions.insert(self, ind - 1)

    def move_right(self):
        ind = self.subcaptions.find(self)
        self.subcaptions.remove(self)
        self.subcaptions.insert(self, ind + 1)

@core.windows.hooks.split (900)
@core.windows.hooks.select(900)
def update_two(w1, w2):
    SubCaption.update(w1)
    SubCaption.update(w2)


@core.windows.hooks.create(900)
def update_one(w1):
    SubCaption.update(w1)

import core.windows
import core.ui

class WinCaption:
    wincaptions = {}

    def __init__(self, w):
        self.parts  = []
        self.window = w
        self.wincaptions[w] = self

    @classmethod
    def updatesub(self, sub):
        for caption in self.wincaptions.values():
            caption.generate(sub)
            caption.draw()

    def generate(self, sub):
        index = SubCaption.subcaptions.index(sub)

        while len(self.parts) <= index:
            self.parts.append("")

        self.parts[index] = sub.generate(self.window)

    def draw(self):
        self.window.caption = " ".join(filter(None, self.parts))
        core.ui.nc_refresh()

class SubCaption:
    subcaptions = []

    def __init__(self, *hooks):
        self.hookfuncts = []

        for hook in hooks:
            self.addhookfunct(hook)

        self.subcaptions.append(self)

    def __call__(self, funct):
        self.funct = funct

        for cap in WinCaption.wincaptions.values():
            cap.generate(self)
            cap.draw()

    def addhookfunct(self, hook):
        def hf(*args):
            args = filter(lambda a: isinstance(a, core.windows.Window),
                          args)

            for w in args:
                for sub in w:
                    if w not in WinCaption.wincaptions:
                        continue

                    caption = WinCaption.wincaptions[w]
                    caption.updatesub(self)
                    caption.draw()

        self.hookfuncts.append(hf)

        hook.mount(hf, 800)

    def generate(self, win):
        return self.funct(win)

@core.windows.hooks.delete_pre(100)
def handle_window_delete(w):
    if w in WinCaption.wincaptions:
        del   WinCaption.wincaptions[w]

@core.windows.hooks.create(100)
@core.windows.hooks.split(100)
def handle_window_change(*args):
    args = filter(lambda w: isinstance(w, core.windows.Window), args)

    for w in args:
        if w.isleaf():
            if w not in WinCaption.wincaptions:
                cap = WinCaption(w)

                for sub in SubCaption.subcaptions:
                    cap.generate(sub)

                cap.draw()

        else:
            if w in WinCaption.wincaptions:
                del WinCaption.wincaptions[w]

handle_window_change(*list(core.windows.get_root()))

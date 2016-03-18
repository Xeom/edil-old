import c.io
import systems.hook

class hooks:
    keypress = None

def initsys():
    c.io.initsys()

    hooks.keypress = systems.hook.Hook(c.io.on_keypress, c.io.key_s)

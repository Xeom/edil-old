import core.windows
import core.keymap

from core.key import Key

core.keymap.maps.add("window-default")
winmap = core.keymap.maps["window-default"]

modifier_key = Key("W", con=True)

@winmap.add(modifier_key, Key("i"))
def winsplitup(keys):
    core.windows.get_selected().split(core.windows.direction.up)

@winmap.add(modifier_key, Key("k"))
def winsplitdown(keys):
    core.windows.get_selected().split(core.windows.direction.down)

@winmap.add(modifier_key, Key("j"))
def winsplitleft(keys):
    core.windows.get_selected().split(core.windows.direction.left)

@winmap.add(modifier_key, Key("l"))
def winsplitright(keys):
    core.windows.get_selected().split(core.windows.direction.right)

@winmap.add(modifier_key, Key("c"))
def windel(keys):
    core.windows.get_selected().delete()

@winmap.add(modifier_key, Key("."))
def winnext(keys):
    core.windows.get_selected().next().select()

@winmap.add(modifier_key, Key(","))
def winprev(keys):
    core.windows.get_selected().prev().select()

@winmap.add(modifier_key, Key("m"))
def winup(keys):
    core.windows.get_selected().parent.select()

#@winmap.add(Key("DOWN",  esc=True))
@winmap.add(modifier_key, Key("o"))
def winadjplus(keys):
    core.windows.get_selected().adj(1)

@winmap.add(modifier_key, Key("O", con=True))
def winadjlongplus(keys):
    core.windows.get_selected().adj(8)

    
#@winmap.add(Key("UP",   esc=True))
@winmap.add(modifier_key, Key("u"))
def winadjminus(keys):
    core.windows.get_selected().adj(-1)

@winmap.add(modifier_key, Key("U", con=True))
def winadjminus(keys):
    core.windows.get_selected().adj(-8)

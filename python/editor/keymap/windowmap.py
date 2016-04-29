import core.windows
import core.keymap

from core.key import Key

core.keymap.maps.add("window-default")
winmap = core.keymap.maps["window-default"]

@winmap.add(Key("W", con=True), Key("s"), Key("UP"))
def winsplitup(keys):
    core.windows.get_selected().split(core.windows.direction.up)

@winmap.add(Key("W", con=True), Key("s"), Key("DOWN"))
def winsplitdown(keys):
    core.windows.get_selected().split(core.windows.direction.down)

@winmap.add(Key("W", con=True), Key("s"), Key("LEFT"))
def winsplitleft(keys):
    core.windows.get_selected().split(core.windows.direction.left)

@winmap.add(Key("W", con=True), Key("s"), Key("RIGHT"))
def winsplitright(keys):
    core.windows.get_selected().split(core.windows.direction.right)

@winmap.add(Key("W", con=True), Key("x"))
def windel(keys):
    core.windows.get_selected().delete()

@winmap.add(Key("W", con=True), Key("RIGHT"))
def winnext(keys):
    core.windows.get_selected().next().select()

@winmap.add(Key("W", con=True), Key("LEFT"))
def winprev(keys):
    core.windows.get_selected().prev().select()

@winmap.add(Key("W", con=True), Key("UP"))
def winup(keys):
    core.windows.get_selected().parent.select()

#@winmap.add(Key("DOWN",  esc=True))
@winmap.add(Key("W", con=True), Key("RIGHT", esc=True))
def winadjplus(keys):
    core.windows.get_selected().adj(1)

#@winmap.add(Key("UP",   esc=True))
@winmap.add(Key("W", con=True), Key("LEFT", esc=True))
def winadjminus(keys):
    core.windows.get_selected().adj(-1)


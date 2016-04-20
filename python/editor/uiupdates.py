from core.buffer  import hooks as bhooks
from core.windows import hooks as whooks

import core.ui

import sys

@whooks.select(50)
def handle_win_select(new, old):
    core.ui.draw_window_frame(new)
    core.ui.draw_window_frame(old)

@whooks.resize_x(50)
@whooks.resize_y(50)
def handle_win_resize(w, x, y):
    core.ui.draw_window(w)
    core.ui.nc_refresh()

@whooks.create(50)
def handle_win_split(w):
    core.ui.draw_window(w)
    core.ui.nc_refresh()

@whooks.delete_post(50)
def handle_win_delete(w):
    core.ui.draw_window(w)
    core.ui.nc_refresh()

@bhooks.line.change_post(50)
def handle_line_change(b, ln, v):
    core.ui.draw_buffer_line(b, ln)
    core.ui.nc_refresh()

@bhooks.line.delete_post(50)
def handle_line_delete(b, ln):
    core.ui.draw_buffer(b)
    core.ui.nc_refresh()

@bhooks.line.insert_post(50)
def handle_line_insert(b, ln):
    core.ui.draw_buffer(b)
    core.ui.nc_refresh()


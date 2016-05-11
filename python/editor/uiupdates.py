from core.buffer  import hooks as bhooks
from core.windows import hooks as whooks

import core.ui
import core.windows

holding_buffer = None

def hold_buffer(b):
    global holding_buffer
    if holding_buffer != None:
        release_buffer()

    holding_buffer = b

def release_buffer():
    global holding_buffer
    if holding_buffer == None:
        return

    core.ui.draw_buffer(holding_buffer)
    holding_buffer = None

@whooks.caption.set_post(50)
def handle_caption_set(w, caption):
    if holding_buffer == w.buffer:
        return

    core.ui.draw_window_frame(w)

@whooks.sidebar.set_post(50)
def handle_sidebar_set(w, sidebar):
    if holding_buffer == w.buffer:
        return

    core.ui.draw_window_frame(w)

@whooks.select(50)
def handle_win_select(new, old):
    core.ui.draw_window_frame(new)
    core.ui.draw_window_frame(old)

@whooks.size.adj_post(50)
def handle_win_adj(w, adj):
    if holding_buffer == w.buffer:
        return

    core.ui.draw_window(w)
    core.ui.nc_refresh()

@whooks.create(50)
def handle_win_split(w):
    if holding_buffer == w.buffer:
        return

    core.ui.draw_window(w)
    core.ui.nc_refresh()

@whooks.buffer_set(50)
def handle_win_buffer_set(w, b):
    if holding_buffer == w.buffer:
        return

    core.ui.draw_window(w)
    core.ui.nc_refresh()

@whooks.delete_post(50)
def handle_win_delete(w):
    if holding_buffer == w.buffer:
        return

    core.ui.draw_window(w)
    core.ui.nc_refresh()

@bhooks.line.change_post(50)
def handle_line_change(b, ln, v):
    if holding_buffer == b:
        return

    core.ui.draw_buffer_line(b, ln)
    core.ui.nc_refresh()

@bhooks.line.delete_post(50)
def handle_line_delete(b, ln):
    if holding_buffer == b:
        return

    core.ui.draw_buffer_after(b, ln)
    core.ui.nc_refresh()

@bhooks.line.insert_post(50)
def handle_line_insert(b, ln):
    if holding_buffer == b:
        return

    core.ui.draw_buffer_after(b, ln)
    core.ui.nc_refresh()

@bhooks.batch_region(50)
def handle_batch_region(b, start, end):
    if holding_buffer == b:
        return

    core.ui.draw_buffer_after(b, start)
    core.ui.nc_refresh()

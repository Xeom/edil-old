from core.buffer  import hooks as bhooks
from core.windows import hooks as whooks

import core.ui
import core.windows

@whooks.caption.set_post(50)
def handle_caption_set(w, caption):
    core.ui.draw_window_frame(w)

@whooks.sidebar.set_post(50)
def handle_sidebar_set(w, sidebar):
    core.ui.draw_window_frame(w)

@whooks.select(50)
def handle_win_select(new, old):
    core.ui.draw_window_frame(new)
    core.ui.draw_window_frame(old)

@whooks.size.adj_post(50)
def handle_win_adj(w, adj):
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


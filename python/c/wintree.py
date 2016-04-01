import ctypes
from shared import lib as so

from c.hook    import hook_p

class wincont_s(ctypes.Structure):
	pass

wincont_p = ctypes.POINTER(wincont_s)

class wintree_s(ctypes.Structure):
    pass

wintree_p = ctypes.POINTER(wintree_s)

class dir:
    (
        left,
        right,
        up,
        down
    ) = range(4)

class splitdir:
    (
        lr,
        ud,
        none
    ) = range(3)

#int wintree_initsys(void)
initsys = so.wintree_initsys
initsys.argtypes = []
initsys.restype  = ctypes.c_int

#int wintree_killsys(void)
killsys = so.wintree_killsys
killsys.argtypes = []
killsys.restype  = ctypes.c_int

#int wintree_set_root_size(uint x, uint y)
set_root_size = so.wintree_set_root_size
set_root_size.argtypes = [ctypes.c_uint, ctypes.c_uint]
set_root_size.restype  = ctypes.c_int

#int wintree_set_caption(wintree *tree, const char *caption);
set_caption = so.wintree_set_caption
set_caption.argtypes = [wintree_p, ctypes.c_char_p]
set_caption.restype  = ctypes.c_int

#int wintree_set_sidebar(wintree *tree, const char *sidebar);
set_sidebar = so.wintree_set_sidebar
set_sidebar.argtypes = [wintree_p, ctypes.c_char_p]
set_sidebar.restype  = ctypes.c_int

#int wintree_delete(wintree *tree)
delete = so.wintree_delete
delete.argtypes = [wintree_p]
delete.restype  = ctypes.c_int

#int wintree_move_border(wintree *tree, int n)
move_border = so.wintree_move_border
move_border.argtypes = [wintree_p, ctypes.c_int]
move_border.restype  = ctypes.c_int

#size_t wintree_get_posx(wintree *tree)
get_posx = so.wintree_get_posx
get_posx.argtypes = [wintree_p]
get_posx.restype  = ctypes.c_int

#size_t wintree_get_posy(wintree *tree)
get_posy = so.wintree_get_posy
get_posy.argtypes = [wintree_p]
get_posy.restype  = ctypes.c_int

#wintree *wintree_iter_next(wintree *tree);
iter_next = so.wintree_iter_next
iter_next.argtypes = [wintree_p]
iter_next.restype  = wintree_p

#wintree *wintree_iter_start(void)
iter_start = so.wintree_iter_start
iter_start.argtypes = []
iter_start.restype  = wintree_p

#wintree *wintree_get_selected(void);
get_selected = so.wintree_get_selected
get_selected.argtypes = []
get_selected.restype  = wintree_p

#int wintree_select(wintree *tree)
select = so.wintree_select
select.argtypes = [wintree_p]
select.restype  = ctypes.c_int

#int wintree_split(wintree *tree, windir dir);
split = so.wintree_split
split.argtypes = [wintree_p, ctypes.c_int]
split.restype  = ctypes.c_int

#wincont *wintree_get_content(wintree *tree);
get_buffer = so.wintree_get_buffer
get_buffer.argtypes = [wintree_p]
get_buffer.restype  = wincont_p

#wintree *wintree_get_parent(wintree *tree);
get_parent = so.wintree_get_parent
get_parent.argtypes = [wintree_p]
get_parent.restype  = wintree_p

#uint wintree_get_sizex(wintree *tree);
get_sizex = so.wintree_get_sizex
get_sizex.argtypes = [wintree_p]
get_sizex.restype  = ctypes.c_uint

#uint wintree_get_sizey(wintree *tree);
get_sizey = so.wintree_get_sizey
get_sizey.argtypes = [wintree_p]
get_sizey.restype  = ctypes.c_uint

#char *wintree_get_caption(wintree *tree);
get_caption = so.wintree_get_caption
get_caption.argtypes = [wintree_p]
get_caption.restype  = ctypes.c_char_p

#char *wintree_get_sidebar(wintree *tree);
get_sidebar = so.wintree_get_caption
get_sidebar.argtypes = [wintree_p]
get_sidebar.restype  = ctypes.c_char_p

on_resizex = ctypes.cast(so.wintree_on_resizex, hook_p)
on_resizey = ctypes.cast(so.wintree_on_resizey, hook_p)
on_create  = ctypes.cast(so.wintree_on_create,  hook_p)
on_delete  = ctypes.cast(so.wintree_on_delete,  hook_p)

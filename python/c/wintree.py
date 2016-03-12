import ctypes
from shared import lib as so
from c.wincont import wincont_p

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

#wintree *wintree_init(wincont *content)
#init = so.wintree_init
#init.argtypes = [c.wincont.wincont_p]
#sinit.restype  = wintree_p

#int wintree_set_root_size(size_t x, size_t y)
set_root_size = so.wintree_set_root_size
set_root_size.argtypes = [ctypes.c_size_t, ctypes.c_size_t]
set_root_size.restype  = ctypes.c_int

#int wintree_delete(wintree *tree)
delete = so.wintree_delete
delete.argtypes = [wintree_p]
delete.restype  = ctypes.c_int

#int wintree_swap_next(wintree *tree)
swap_next = so.wintree_swap_next
swap_next.argtypes = [wintree_p]
swap_next.restype  = ctypes.c_int

#int wintree_swap_prev(wintree *tree)
swap_prev = so.wintree_swap_prev
swap_prev.argtypes = [wintree_p]
swap_prev.restype  = ctypes.c_int

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

#wintree *wintree_get_selected(void);
get_selected = so.wintree_get_selected
get_selected.argtypes = []
get_selected.restype  = wintree_p

#int wintree_select_next(wintree *tree)
select_next = so.wintree_select_next
select_next.argtypes = [wintree_p]
select_next.restype  = ctypes.c_int

#int wintree_split(wintree *tree, windir dir);
split = so.wintree_split
split.argtypes = [wintree_p, ctypes.c_int]
split.restype  = ctypes.c_int

#wincont *wintree_get_content(wintree *tree);
get_content = so.wintree_get_content
get_content.argtypes = [wintree_p]
get_content.restype  = ctypes.c_int

#wintree *wintree_get_parent(wintree *tree);
get_parent = so.wintree_get_parent
get_parent.argtypes = [wintree_p]
get_parent.restype  = ctypes.c_int

#size_t wintree_get_sizex(wintree *tree);
get_sizex = so.wintree_get_sizex
get_sizex.argtypes = [wintree_p]
get_sizex.restype  = ctypes.c_int

#size_t wintree_get_sizey(wintree *tree);
get_sizey = so.wintree_get_sizey
get_sizey.argtypes = [wintree_p]
get_sizey.restype  = ctypes.c_int

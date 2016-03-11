from shared import lib as so

class wincont_s(ctypes.Structure):
    pass

wincont_p = ctypes.POINTER(wincontkz_s)

# int wincont_initsys(void);-
initsys = so.wincont_initsys
initsys.argtypes = []
initsys.restype  = ctypes.c_int

# wincont *wincont_init(textcont *text);
init = so.wincont_init
init.argtypes = [textcont_p]
init.restype  = wincont_p

# wincont *wincont_clone(wincont *cont);
clone = so.wincont_clone
clone.argtypes = [wincont_p]
clone.restype  = wincont_p

# wincont *wincont_next(wincont *cont);
next = so.wincont_next
next.argtypes = [wincont_p]
next.restype  = wincont_p

# wincont *wincont_prev(wincont *cont);
prev = so.wincont_prev
prev.argtypes = [wincont_p]
prev.restype  = wincont_p

# wincont *wincont_get_root(void);
get_root = so.wincont_get_root
get_root.argtypes = []
get_root.restype  = wincont_p

# line *wincont_get_line(wincont *cont, lineno ln);
get_line = so.wincont_get_line
get_line.argtypes = [wincont_p, ctypes.c_size_t]
get_line.restype  = line_p

# const char *wincont_get_line_text_const(wincont *cont, line *l);
get_line_text_const = so.wincont_get_line_text_const
get_line_text_const.argtypes = [wincont_p, line_p]
get_line_text_const.restype  = ctypes.c_char_p

# textcont *wincont_get_textcont(wincont *cont);
get_textcont = so.wincont_get_textcont
get_textcont.argtypes = [wincont_p]
get_textcont.restype  = textcont_p

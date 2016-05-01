import ctypes
from shared import lib as so

refresh = so.refresh
refresh.argtypes = []
refresh.restype  = ctypes.c_int

endwin= so.endwin
endwin.argtypes = []
endwin.restype  = ctypes.c_int

getch = so.getch
getch.argtypes = []
getch.restype  = ctypes.c_int

clear = so.clear
clear.argtypes = []
clear.restype  = ctypes.c_int

free = so.free
free.argtypes = [ctypes.c_void_p]

move = so.move
move.argtypes = [ctypes.c_int, ctypes.c_int]

import ctypes
from shared import lib as so

class textcont_s(ctypes.Structure):
    pass

textcont_p = ctypes.POINTER(textcont_s)

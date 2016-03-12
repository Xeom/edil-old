import ctypes
from shared import lib as so

class line_s(ctypes.Structure):
    pass

line_p = ctypes.POINTER(line_s)

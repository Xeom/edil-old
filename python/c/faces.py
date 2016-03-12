import ctypes
from shared import lib as so

class face_s(ctypes.Structure):
    pass

face_p = ctypes.POINTER(face_s)

initsys = so.face_initsys
initsys.argtypes = []
initsys.restype  = None

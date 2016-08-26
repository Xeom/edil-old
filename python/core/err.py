import sys
import traceback

import symbols.err

class Level(symbols.err.err_lvl):
    pass

def traced_new(lvl=Level.critical):
    typ, exc, tb = sys.exc_info()
    detail = "".join(traceback.format_tb(tb))

    new(lvl, "Caught Python Exception", tb + str(exc))

def raise_new(exc, lvl=Level.critical):
    tb = "".join(traceback.format_stack()[:-1])

    new(lvl, details=tb + str(exc))

    raise exc

def new(lvl=Level.medium, title="Python Exception", details=None):
    symbols.err.new(lvl, title, details)

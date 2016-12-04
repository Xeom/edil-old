import sys
import traceback

import symbols.err

class Level(symbols.err.err_lvl):
    pass

def traced_new(lvl=Level.critical):
    typ, exc, tb = sys.exc_info()
    detail = "".join(traceback.format_tb(tb))

    new(
        lvl,
        "Caught Python Exception",
        "\n".join(traceback.format_exception(typ, None, tb))
    )

def new(lvl=Level.medium, title="Python Exception", details=None):
    if isinstance(title, str):
        title = title.encode("utf-8")

    if isinstance(details, str):
        details = details.encode("utf-8")

    symbols.err.new(lvl, title, details)

def set_care_lvl(lvl):
    symbols.err.min_care_lvl.value = lvl;

def set_detail_lvl(lvl):
    symbols.err.min_detail_lvl.value = lvl;

def set_quit_lvl(lvl):
    symbols.err.min_quit_lvl.value = lvl;

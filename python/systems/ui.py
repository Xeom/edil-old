import c.ui
import c.lib
import signal

from decorators.command import Command

def initsys():
    c.ui.initsys()
    c.ui.resize()

   # signal.signal(
  #      signal.SIGWINCH,
 #       resize
#    )

def resize():
    import sys
    c.ui.resize()
    sys.stderr.write("Did resize\n")
    refresh()

def refresh():
    c.lib.clear()
    c.ui.display_wintrees()
    c.lib.refresh()

    
def killsys():
    c.lib.endwin()
    c.ui.killsys()

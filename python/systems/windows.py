import c.wincont
import c.wintree

class direction(c.wintree.dir):
    pass

def initsys():
    c.wincont.initsys()
    c.wintree.initsys()

def split(direction):
    c.wintree.split(c.wintree.get_selected(), direction)

def select_next():
    c.wintree.select_next(c.wintree.get_selected())

def delete():
    c.wintree.delete(c.wintree.get_selected())

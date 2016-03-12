import curses

stdscr = curses.initscr()
curses.noecho()
curses.cbreak()
stdscr.keypad(True)

class Pos:
	def __init__(self, x, y):
		self.x, self.y = x, y

	@property
	def tuple(self):
		return (self.x, self.y)

	@tuple.setter
	def tuple(self, new):
		self.x, self.y = new

class Screen:
	def __init__(self):
		self.scr = cruses.initscr()
		curses.noecho()
		curses.cbreak()
		self.scr.keypad(True)

class UDSplit:
	def __init__(self, parent, up, down):
		self.up   = None
		self.down = None

	def splitchildlr(self, child):
		if child == self.up:
			self.up = LRSplit(self, 
class LRSplit:
	def __init__(self, parent=None):
		self.parent = parent
		self.children = []


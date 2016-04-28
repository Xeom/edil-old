from editor.cursor.point import Point

class RegionCursor:
    def __init__(self, buffer):
        self.buffer = buffer
        self.active = False

        self.anchor = Point(buffer)
        self.runner = Point(buffer)

    def move_cols(self, n):
        self.runner.move_cols(n)

    def move_lines(self, n):
        self.runner.move_lines(n)

    def swap(self):
        self.anchor, self.runner = \
        self.runner, self.anchor

    def insert_char(self, char):
        if self.active:
            self.region_delete()

        self.runner.insert_char(char)

    def delete_char(self):
        if self.active:
            self.region_delete()
            self.deactivate()

        else:
            self.runner.delete_char()

    def enter(self):
        if self.active:
            self.region_delete()
            self.deactivate()

        self.runner.enter()

    def activate(self):
        self.anchor.cn = self.runner.cn
        self.anchor.ln = self.runner.ln

        self.active = True

    def deactivate(self):
        self.active = False

    def region_delete(self):
        start = self.start
        end   = self.end

        while end > start:
            end.delete_char()

    @property
    def start(self):
        return min(self.anchor, self.runner)

    @property
    def end(self):
        return max(self.anchor, self.runner)

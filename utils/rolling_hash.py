class RollingHash:
    def __init__(self, base, length):
        self.h = 0
        self.base = int(base)
        self.mod = int(1e9 + 7)
        self.most_significant = int(pow(base, length - 1, self.mod))

    def roll(self, new, last):
        self.h = (self.h - (self.most_significant * int(last)) %
                  self.mod) % self.mod
        self.h = ((self.h * self.base) + int(new)) % self.mod

    def __hash__(self):
        return self.h

from collections import defaultdict

import sys

sys.setrecursionlimit(2000)


class MTFNode:
    def __init__(self) -> None:
        self.indices = set()
        self.children = defaultdict(MTFNode)

    def add(self, buffer, cur, index):
        if cur < len(buffer) - 1:
            self.children[buffer[cur]].add(buffer, cur + 1, index)
        self.children[buffer[cur]].indices.add(index)

    def remove(self, buffer, cur, index):
        if index in self.indices:
            self.indices.remove(index)
        if cur < len(buffer) - 1:
            self.children[buffer[cur]].remove(buffer, cur + 1, index)
            if len(self.children[buffer[cur]].indices) == 0:
                del self.children[buffer[cur]]

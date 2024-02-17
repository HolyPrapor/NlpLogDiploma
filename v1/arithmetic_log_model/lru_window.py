class LinkedNode(object):
    def __init__(self, key):
        self.key = key
        self.next = None
        self.prev = None


class LRUWindow:
    def __init__(self):
        self.lookup = {}
        self.dummy = LinkedNode(0)
        self.head = self.dummy.next
        self.tail = self.dummy.next

    def _remove_head_node(self):
        if not self.head:
            return
        prev = self.head
        self.head = self.head.next
        if self.head:
            self.head.prev = None
        del prev

    def _append_new_node(self, new_node):
        if not self.tail:
            self.head = self.tail = new_node
        else:
            self.tail.next = new_node
            new_node.prev = self.tail
            self.tail = self.tail.next

    def _unlink_cur_node(self, node):
        """unlink current linked node"""
        if self.head is node:
            self.head = node.next
            if node.next:
                node.next.prev = None
            return

        prev, nex = node.prev, node.next
        prev.next = nex
        nex.prev = prev

    def move_to_front(self, key: int) -> None:
        node = self.lookup[key]
        if node is not self.tail:
            self._unlink_cur_node(node)
            self._append_new_node(node)

    def append(self, key: int) -> int:
        if key in self.lookup:
            raise RuntimeError("Key already exists")

        new_node = LinkedNode(key)
        self.lookup[key] = new_node
        self._append_new_node(new_node)

    def pop(self) -> int:
        val = self.head.key
        self.lookup.pop(self.head.key)
        self._remove_head_node()
        return val

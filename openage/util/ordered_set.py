# Copyright 2019-2019 the openage authors. See copying.md for legal info.

"""
Provides a very simple implementation of an ordered set. We use the
Python dictionaries as a basis because they are guaranteed to
be ordered since Python 3.6.
"""


class OrderedSet:
    """
    Set that saves the input order of elements.
    """

    def __init__(self):
        self.ordered_set = dict()

    def append_left(self, elem):
        """
        Add an element to the front of the set.
        """
        if elem not in self.ordered_set:
            temp_set = {elem: True}
            self.ordered_set = temp_set.update(self.ordered_set)

    def append_right(self, elem):
        """
        Add an element to the back of the set.
        """
        self.ordered_set[elem] = True

    def discard(self, elem):
        """
        Remove an element from the set.
        """
        self.ordered_set.pop(elem, False)

    def intersect(self, other):
        """
        Only keep elements that are both in self and other.
        """
        keys_self = set(self.ordered_set.keys())
        keys_other = set(other.keys())
        intersection = keys_self & keys_other

        for elem in self:
            if elem not in intersection:
                self.discard(elem)

    def __contains__(self, elem):
        return elem in self.ordered_set

    def __iter__(self):
        return iter(self.ordered_set)

    def __len__(self):
        return len(self.ordered_set)

    def __reversed__(self):
        return reversed(self.ordered_set)

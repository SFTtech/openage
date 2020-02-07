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

    def __init__(self, elements=None):
        self.ordered_set = {}

        if elements:
            self.update(elements)

    def add(self, elem):
        """
        Set-like add that calls append_right().
        """
        self.append_right(elem)

    def append_left(self, elem):
        """
        Add an element to the front of the set.
        """
        if elem not in self.ordered_set:
            temp_set = {elem: 0}

            # Update indices
            for key in self.ordered_set:
                self.ordered_set[key] += 1

            temp_set.update(self.ordered_set)
            self.ordered_set = temp_set

    def append_right(self, elem):
        """
        Add an element to the back of the set.
        """
        if elem not in self.ordered_set:
            self.ordered_set[elem] = len(self)

    def discard(self, elem):
        """
        Remove an element from the set.
        """
        index = self.ordered_set.pop(elem, -1)

        if index > -1:
            # Update indices
            for key, value in self.ordered_set.items():
                if value > index:
                    self.ordered_set[key] -= 1

    def get_list(self):
        """
        Returns a normal list containing the values from the ordered set.
        """
        return list(self.ordered_set.keys())

    def index(self, elem):
        """
        Returns the index of the element in the set or
        -1 if it is not in the set.
        """
        if elem in self.ordered_set:
            return self.ordered_set[elem]

        return -1

    def intersection_update(self, other):
        """
        Only keep elements that are both in self and other.
        """
        keys_self = set(self.ordered_set.keys())
        keys_other = set(other.keys())
        intersection = keys_self & keys_other

        for elem in self:
            if elem not in intersection:
                self.discard(elem)

    def union(self, other):
        """
        Returns a new ordered set with the elements from self and other.
        """
        element_list = self.get_list() + other.get_list()
        return OrderedSet(element_list)

    def update(self, other):
        """
        Append the elements of another iterable to the right of the
        ordered set.
        """
        for elem in other:
            self.append_right(elem)

    def __contains__(self, elem):
        return elem in self.ordered_set

    def __iter__(self):
        return iter(self.ordered_set)

    def __len__(self):
        return len(self.ordered_set)

    def __reversed__(self):
        return reversed(self.ordered_set)

    def __str__(self):
        return f'OrderedSet({list(self.ordered_set.keys())})'

    def __repr__(self):
        return str(self)

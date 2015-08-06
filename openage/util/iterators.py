# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Provides all sorts of iterator-related stuff.
"""


class ListIterator:
    """
    An iterator type for list-like objects, i.e. objects
    that implement __getitem__.
    """

    # pylint: disable=too-few-public-methods

    def __init__(self, listobject):
        self.pos = 0
        self.listobject = listobject

    def __iter__(self):
        return self

    def __next__(self):
        try:
            result = self.listobject[self.pos]
            self.pos += 1
            return result
        except IndexError:
            raise StopIteration from None


def denote_last(iterable):
    """
    Similar to enumerate, this iterates over an iterable, and yields
    tuples of item, is_last.

    is_last is True for the last item before StopIteration, and False
    for all others.
    """
    iterator = iter(iterable)

    next_ = next(iterator)
    is_last = False

    while True:
        current = next_
        try:
            next_ = next(iterator)
        except StopIteration:
            is_last = True

        yield current, is_last

        if is_last:
            raise StopIteration

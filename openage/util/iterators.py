# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Provides all sorts of iterator-related stuff.
"""


def denote_last(iterable):
    """
    Similar to enumerate, this iterates over an iterable, and yields
    tuples of item, is_last.
    """
    iterator = iter(iterable)
    current = next(iterator)

    for future in iterator:
        yield current, False
        current = future

    """Exhausted iterator so current is the last item."""
    yield current, True

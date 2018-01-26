# Copyright 2015-2018 the openage authors. See copying.md for legal info.

"""
Provides all sorts of iterator-related stuff.
"""


def denote_last(iterable):
    """
    Similar to enumerate, this iterates over an iterable, and yields
    tuples of item, is_last.
    """
    # pylint: disable=stop-iteration-return
    iterator = iter(iterable)
    current = next(iterator)

    for future in iterator:
        yield current, False
        current = future

    yield current, True

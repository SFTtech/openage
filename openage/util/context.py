# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Provides some utility context guards.
"""


class DummyGuard:
    """ Context guard that does nothing. """
    # pylint: disable=too-few-public-methods

    @staticmethod
    def __enter__():
        pass

    @staticmethod
    def __exit__(exc_type, exc_value, traceback):
        del exc_type, exc_value, traceback  # unused

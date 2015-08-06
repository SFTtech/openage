# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Some utility function decorators
"""


def run_once(func):
    """
    Decorator to run func only at its first invocation.

    Set func.has_run to False to manually re-run.
    """

    def wrapper(*args, **kwargs):
        """ Returned function wrapper. """
        if not wrapper.has_run:
            wrapper.has_run = True
            return func(*args, **kwargs)

    wrapper.has_run = False
    return wrapper

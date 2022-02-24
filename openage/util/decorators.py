# Copyright 2015-2022 the openage authors. See copying.md for legal info.

"""
Some utility function decorators
"""


from typing import Callable


def run_once(func: Callable) -> Callable:
    """
    Decorator to run func only at its first invocation.

    Set func.has_run to False to manually re-run.
    """

    def wrapper(*args, **kwargs):
        """ Returned function wrapper. """
        if wrapper.has_run:
            return None

        wrapper.has_run = True
        return func(*args, **kwargs)

    wrapper.has_run = False
    return wrapper

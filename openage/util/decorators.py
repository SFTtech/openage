# Copyright 2015-2022 the openage authors. See copying.md for legal info.

"""
Some utility function decorators
"""

from collections.abc import Callable
from typing import Any, Generic, TypeVar

T = TypeVar("T")


class _RunOnce(Generic[T]):
    """
    Callable that only invokes its function on the first call.
    """

    has_run: bool

    def __init__(self, func: Callable[..., T]):
        self.func = func
        self.has_run = False

    def __call__(self, *args, **kwargs) -> T | None:
        if self.has_run:
            return None

        self.has_run = True
        return self.func(*args, **kwargs)


def run_once(func: Callable[..., Any]) -> Callable[..., Any]:
    """
    Decorator to run func only at its first invocation.

    Set func.has_run to False to manually re-run.
    """

    return _RunOnce(func)

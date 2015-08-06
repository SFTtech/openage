# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Performs doctest on all modules listed in list_doctest_modules().
"""

import importlib
from doctest import testmod

from .testing import TestError
from .testlist import doctest_modules


def test():
    """
    Runs doctest on the modules listed in testlist.DOCTEST_MODULES.
    """
    for modname in doctest_modules():
        mod = importlib.import_module(modname)
        if testmod(mod, report=False).failed:
            raise TestError("Errors have been detected during doctest")

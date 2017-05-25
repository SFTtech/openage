# Copyright 2013-2016 the openage authors. See copying.md for legal info.

"""
The Python part of openage, a free engine re-write of
Age of Empires II.

See https://openage.sft.mx and http://github.com/sfttech/openage.

Requires Python 3.4.
"""

from sys import version_info as py_version

from .util.version import get_version


if py_version < (3, 4):
    raise Exception("openage requires python 3.4 or higher.")

VERSION = (0, 3, 1, 'alpha', 0)

__version__ = get_version(VERSION)

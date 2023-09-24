# Copyright 2015-2016 the openage authors. See copying.md for legal info.

"""
Wraps openage::pyinterface::check, which checks whether setup has
properly initialized all interface components.
"""

from libopenage.pyinterface.setup cimport check as check_cpp


def check():
    check_cpp()

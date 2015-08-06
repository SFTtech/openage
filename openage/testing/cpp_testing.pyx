# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Provides a wrapper for openage::testing::run_method.
"""

from libopenage.testing.testlist cimport run_method as run_method


def run_cpp_method(str methodname):
    """ Runs the given C++ void () method, by its name. """
    run_method(methodname.encode())

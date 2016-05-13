# Copyright 2016-2016 the openage authors. See copying.md for legal info.

"""
setuptools wrapper module for building openage's python library.
"""

from setuptools import setup, find_packages, Extension as OriginalExtension

from .dist import OpenageDistribution


# pylint: disable=R0903
class Extension(OriginalExtension):
    """Custom extension class which provides some sensible defaults"""

    def __init__(self, *args, **kwargs):
        kwargs.setdefault('language', 'c++')
        OriginalExtension.__init__(self, *args, **kwargs)


__all__ = ['setup', 'find_packages', 'Extension', 'OpenageDistribution']

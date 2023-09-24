# Copyright 2017-2017 the openage authors. See copying.md for legal info.

from libopenage.util.path cimport Path


cdef cpppath_to_pypath(const Path &path)

# Copyright 2017-2017 the openage authors. See copying.md for legal info.

"""
Functions called from C++ to perform method calls on
filelike python objects.
"""


from cpython.ref cimport PyObject
from libcpp cimport bool
from libcpp.string cimport string


from libopenage.util.filelike.filelike_python cimport (
    pyx_file_read,
)


cdef string file_read(PyObject *filelike, ssize_t max) except * with gil:
    return (<object> filelike).read(max)


def setup():
    pyx_file_read.bind0(file_read)

# Copyright 2017-2017 the openage authors. See copying.md for legal info.

from cpython.ref cimport PyObject
from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector

from libopenage.util.fslike.path cimport Path as Path_cpp
from libopenage.util.fslike.abstract cimport fs_is_file, fs_is_dir


cdef vec_decode(vector[string]& items) with gil:
    return [item.decode() for item in items]


cdef bool fs_is_file_py(PyObject *fslike,
                        vector[string]& parts) except * with gil:
    return (<object> fslike).is_file(vec_decode(parts))


cdef bool fs_is_dir_py(PyObject *fslike,
                       vector[string]& parts) except * with gil:
    return (<object> fslike).is_dir(vec_decode(parts))


def setup():
    fs_is_file.bind0(fs_is_file_py)
    fs_is_dir.bind0(fs_is_dir_py)

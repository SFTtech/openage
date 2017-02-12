# Copyright 2017-2017 the openage authors. See copying.md for legal info.

"""
Functions called from C++ to perform method calls on
filelike python objects.
"""

import os

from cpython.ref cimport PyObject
from libc.stdint cimport uint64_t
from libcpp cimport bool
from libcpp.cast cimport static_cast
from libcpp.memory cimport shared_ptr
from libcpp.string cimport string
from libcpp.vector cimport vector

from libopenage.util.file cimport File as File_cpp
from libopenage.util.fslike.python cimport (
    pyx_fs_is_file,
    pyx_fs_is_dir,
    pyx_fs_writable,
    pyx_fs_list,
    pyx_fs_mkdirs,
    pyx_fs_open_r,
    pyx_fs_open_w,
    pyx_fs_get_native_path,
    pyx_fs_rename,
    pyx_fs_rmdir,
    pyx_fs_touch,
    pyx_fs_unlink,
    pyx_fs_get_mtime,
    pyx_fs_get_filesize,
)
from libopenage.pyinterface.pyobject cimport PyObj



cdef bool fs_is_file(PyObject *fslike,
                     const vector[string]& parts) except * with gil:
    return (<object> fslike).is_file(parts)


cdef bool fs_is_dir(PyObject *fslike,
                    const vector[string]& parts) except * with gil:
    return (<object> fslike).is_dir(parts)


cdef bool fs_writable(PyObject *fslike,
                      const vector[string]& parts) except * with gil:
    return (<object> fslike).writable(parts)


cdef vector[string] fs_list(PyObject *fslike, const vector[string]& parts) except * with gil:
    return (<object> fslike).list(parts)


cdef bool fs_mkdirs(PyObject *fslike, const vector[string]& parts) except * with gil:
    return (<object> fslike).mkdirs(parts)


cdef File_cpp fs_open_r(PyObject *fslike, const vector[string]& parts) except * with gil:
    open_path = (<object> fslike).resolve_r(parts)

    if open_path is None:
        raise FileNotFoundError("file could not be found")

    cdef PyObj ref

    native_path = open_path.get_native_path()
    if native_path is not None:
        # open it in c++, 0=read
        return File_cpp(native_path, 0)

    else:
        # open it the python-way
        # and wrap it
        filelike = native_path.open("rb")
        ref = PyObj(<PyObject*> filelike)
        return File_cpp(ref)


cdef File_cpp fs_open_w(PyObject *fslike, const vector[string]& parts) except * with gil:
    open_path = (<object> fslike).resolve_w(parts)

    if open_path is None:
        raise FileNotFoundError("file could not be found")

    cdef PyObj ref

    native_path = open_path.get_native_path()
    if native_path is not None:
        # open it in c++, 1=write
        return File_cpp(native_path, 1)

    else:
        filelike = native_path.open("rb")
        ref = PyObj(<PyObject*> filelike)
        return File_cpp(ref)


cdef PyObj fs_get_native_path(PyObject *fslike,
                              const vector[string]& parts) except * with gil:

    path = (<object> fslike).get_native_path(parts)
    return PyObj(<PyObject*>path)


cdef bool fs_rename(PyObject *fslike,
                    const vector[string]& parts,
                    const vector[string]& target_parts) except * with gil:

    return (<object> fslike).rename(parts, target_parts)


cdef bool fs_rmdir(PyObject *fslike, const vector[string]& parts) except * with gil:
    return (<object> fslike).rmdir(parts)


cdef bool fs_touch(PyObject *fslike, const vector[string]& parts) except * with gil:
    return (<object> fslike).touch(parts)


cdef bool fs_unlink(PyObject *fslike, const vector[string]& parts) except * with gil:
    return (<object> fslike).unlink(parts)


cdef int fs_get_mtime(PyObject *fslike, const vector[string]& parts) except * with gil:
    return (<object> fslike).mtime(parts)


cdef uint64_t fs_get_filesize(PyObject *fslike, const vector[string]& parts) except * with gil:
    return (<object> fslike).filesize(parts)


def setup():
    pyx_fs_is_file.bind0(fs_is_file)
    pyx_fs_is_dir.bind0(fs_is_dir)
    pyx_fs_writable.bind0(fs_writable)
    pyx_fs_list.bind0(fs_list)
    pyx_fs_mkdirs.bind0(fs_mkdirs)
    pyx_fs_open_r.bind0(fs_open_r)
    pyx_fs_open_w.bind0(fs_open_w)
    pyx_fs_get_native_path.bind0(fs_get_native_path)
    pyx_fs_rename.bind0(fs_rename)
    pyx_fs_rmdir.bind0(fs_rmdir)
    pyx_fs_touch.bind0(fs_touch)
    pyx_fs_unlink.bind0(fs_unlink)
    pyx_fs_get_mtime.bind0(fs_get_mtime)
    pyx_fs_get_filesize.bind0(fs_get_filesize)

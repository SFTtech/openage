# Copyright 2017-2017 the openage authors. See copying.md for legal info.

"""
Functions called from C++ to perform method calls on
filelike python objects.
"""


from cpython.ref cimport PyObject
from libc.stdint cimport uint64_t
from libcpp cimport bool
from libcpp.memory cimport shared_ptr
from libcpp.string cimport string
from libcpp.vector cimport vector

from libopenage.util.path cimport Path as Path_cpp
from libopenage.util.file cimport File as File_cpp
from libopenage.util.fslike.fslike_python cimport (
    pyx_fs_is_file,
    pyx_fs_is_dir,
    pyx_fs_writable,
    pyx_fs_list,
    pyx_fs_mkdirs,
    pyx_fs_open_r,
    pyx_fs_open_w,
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


cdef shared_ptr[File_cpp] fs_open_r(PyObject *fslike, const vector[string]& parts) except * with gil:
    filelike = (<object> fslike).open_r(parts)
    cdef PyObj ref = PyObj(<PyObject*>filelike)
    return shared_ptr[File_cpp](new File_cpp(ref))


cdef shared_ptr[File_cpp] fs_open_w(PyObject *fslike, const vector[string]& parts) except * with gil:
    filelike = (<object> fslike).open_w(parts)
    cdef PyObj ref = PyObj(<PyObject*>filelike)
    return shared_ptr[File_cpp](new File_cpp(ref))


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
    pyx_fs_rename.bind0(fs_rename)
    pyx_fs_rmdir.bind0(fs_rmdir)
    pyx_fs_touch.bind0(fs_touch)
    pyx_fs_unlink.bind0(fs_unlink)
    pyx_fs_get_mtime.bind0(fs_get_mtime)
    pyx_fs_get_filesize.bind0(fs_get_filesize)

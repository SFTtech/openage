# Copyright 2017-2018 the openage authors. See copying.md for legal info.

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
from libcpp.utility cimport pair
from libcpp.vector cimport vector

from libopenage.util.file cimport File as File_cpp
from libopenage.util.fslike.fslike cimport FSLike
from libopenage.util.fslike.python cimport (
    Python as FSLikePython,
    pyx_fs_is_file,
    pyx_fs_is_dir,
    pyx_fs_writable,
    pyx_fs_list,
    pyx_fs_mkdirs,
    pyx_fs_open_r,
    pyx_fs_open_w,
    pyx_fs_open_rw,
    pyx_fs_open_a,
    pyx_fs_open_ar,
    pyx_fs_resolve_r,
    pyx_fs_resolve_w,
    pyx_fs_get_native_path,
    pyx_fs_rename,
    pyx_fs_rmdir,
    pyx_fs_touch,
    pyx_fs_unlink,
    pyx_fs_get_mtime,
    pyx_fs_get_filesize,
    pyx_fs_is_fslike_directory,
)
from libopenage.util.path cimport Path as Path_cpp
from libopenage.pyinterface.pyobject cimport PyObj
from .directory import Directory
from .abstract import FSLikeObject
from ..fslike.path import Path as Path_py


cdef class FSLikeCPPWrapper:
    """
    Wraps a c++ fslike object in python.
    This relays the call to the c++ object.

    This fslike object is wrapped again by a pure python class,
    which can then inherit from the FSLikeObject.
    """

    # pointer to the cpp fslike object
    cdef shared_ptr[FSLike] fsobj

    @staticmethod
    cdef wrap(shared_ptr[FSLike] c_fsobj):
        wrp = FSLikeCPPWrapper()
        wrp.fsobj = c_fsobj
        return wrp

    def open_r(self, parts):
        cdef File_cpp file = self.fsobj.get().open_r(parts)
        return None

    def open_w(self, parts):
        cdef File_cpp file = self.fsobj.get().open_w(parts)
        return None

    def resolve_r(self, parts):
        cdef pair[bool, Path] result = self.fsobj.get().resolve_r(parts)

        if not result.first:
            return None
        else:
            return cpppath_to_pypath(result.second)

    def resolve_w(self, parts):
        cdef pair[bool, Path] result = self.fsobj.get().resolve_w(parts)

        if not result.first:
            return None
        else:
            return cpppath_to_pypath(result.second)

    def get_native_path(self, parts):
        cdef string native_path = self.fsobj.get().get_native_path(parts)
        txt = bytes(native_path)

        if txt:
            return txt
        else:
            return None

    def list(self, parts):
        cdef Path.parts_t result = self.fsobj.get().list(parts)

        for entry in result:
            yield from str(entry)

    def filesize(self, parts):
        return self.fsobj.get().get_filesize(parts)

    def mtime(self, parts):
        return self.fsobj.get().get_mtime(parts)

    def mkdirs(self, parts):
        return self.fsobj.get().mkdirs(parts)

    def rmdir(self, parts):
        return self.fsobj.get().rmdir(parts)

    def unlink(self, parts):
        return self.fsobj.get().unlink(parts)

    def touch(self, parts):
        self.fsobj.get().touch(parts)

    def rename(self, srcparts, tgtparts):
        self.fsobj.get().rename(srcparts, tgtparts)

    def is_file(self, parts):
        return self.fsobj.get().is_file(parts)

    def is_dir(self, parts):
        return self.fsobj.get().is_dir(parts)

    def writable(self, parts):
        return self.fsobj.get().writable(parts)


class FSLikeCPP(FSLikeObject):
    """
    Pure python cpp fslike wrapper.
    Wrapps the above translation class again so
    we can inherit from FSLikeObject.
    """

    def __init__(self, cpp_wrapper):
        self.fsobj = cpp_wrapper

    def open_r(self, parts):
        return self.fsobj.open_r(parts)

    def open_w(self, parts):
        return self.fsobj.open_w(parts)

    def resolve_r(self, parts):
        return self.fsobj.resolve_r(parts)

    def resolve_w(self, parts):
        return self.fsobj.resolve_w(parts)

    def get_native_path(self, parts):
        return self.fsobj.get_native_path()

    def list(self, parts):
        yield from self.fsobj.list(parts)

    def filesize(self, parts):
        return self.fsobj.get_filesize(parts)

    def mtime(self, parts):
        return self.fsobj.get_mtime(parts)

    def mkdirs(self, parts):
        return self.fsobj.mkdirs(parts)

    def rmdir(self, parts):
        return self.fsobj.rmdir(parts)

    def unlink(self, parts):
        return self.fsobj.unlink(parts)

    def touch(self, parts):
        self.fsobj.touch(parts)

    def rename(self, srcparts, tgtparts):
        self.fsobj.rename(srcparts, tgtparts)

    def is_file(self, parts):
        return self.fsobj.is_file(parts)

    def is_dir(self, parts):
        return self.fsobj.is_dir(parts)

    def writable(self, parts):
        return self.fsobj.is_writable(parts)


cdef cpppath_to_pypath(const Path_cpp &path):
    cdef FSLike *fsobj = path.get_fsobj();
    cdef FSLikePython *py_fslike

    # we could also use typeid() here, but dat mangling..

    # if the fslike is from python anyway, we can bypass the
    # language barrier (hue hue hue)
    if fsobj.is_python_native():
        # extract the python fslike object and transfer it
        # to the python path
        py_fslike = <FSLikePython *> fsobj
        return Path_py(<object>py_fslike.get_py_fsobj().get_ref(),
                       path.get_parts())

    else:
        # wrap cpp fslike to relay calls
        # then pack it into the python path
        return Path_py(
            FSLikeCPP(
                FSLikeCPPWrapper.wrap(fsobj.shared_from_this()),
                path.get_parts()
            )
        )


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


cdef File_cpp fs_open(object path, int mode) except *:
    if path is None:
        raise Exception("fs_open can't open a path that is None")

    cdef PyObj ref

    native_path = path._get_native_path()
    if native_path is not None:
        # open it in c++, 0=read, 1=write
        return File_cpp(native_path, mode)

    else:
        # sync with filelike/filelike.h enum class mode_t
        # (and the calls to fs_open_* below)
        if mode == 0:
            access_mode = 'rb'
        elif mode == 1:
            access_mode = 'wb'
        elif mode == 2:
            access_mode = 'r+b'
        elif mode == 3:
            access_mode = 'ab'
        elif mode == 4:
            access_mode = 'a+b'
        else:
            raise ValueError("unknown file open mode id: %s" % mode)

        # open it the python-way and wrap it
        filelike = path.open(access_mode)
        ref = PyObj(<PyObject*> filelike)
        return File_cpp(ref)


cdef check_file_exists(object path, object fslike, const vector[string]& parts):
    if path is None:
        raise FileNotFoundError("file could not be found in filesystem %s "
                                "for path '%s'" % (
                                    fslike,
                                    b"/".join(parts).decode(errors='ignore')
                                ))


cdef File_cpp fs_open_r(PyObject *fslike, const vector[string]& parts) except * with gil:
    open_path = (<object> fslike).resolve_r(parts)
    check_file_exists(open_path, <object> fslike, parts)
    return fs_open(open_path, 0)


cdef File_cpp fs_open_w(PyObject *fslike, const vector[string]& parts) except * with gil:
    open_path = (<object> fslike).resolve_w(parts)
    check_file_exists(open_path, <object> fslike, parts)
    return fs_open(open_path, 1)


cdef File_cpp fs_open_rw(PyObject *fslike, const vector[string]& parts) except * with gil:
    open_path = (<object> fslike).resolve_w(parts)
    check_file_exists(open_path, <object> fslike, parts)
    return fs_open(open_path, 2)


cdef File_cpp fs_open_a(PyObject *fslike, const vector[string]& parts) except * with gil:
    open_path = (<object> fslike).resolve_w(parts)
    check_file_exists(open_path, <object> fslike, parts)
    return fs_open(open_path, 3)


cdef File_cpp fs_open_ar(PyObject *fslike, const vector[string]& parts) except * with gil:
    open_path = (<object> fslike).resolve_w(parts)
    check_file_exists(open_path, <object> fslike, parts)
    return fs_open(open_path, 4)


cdef Path_cpp fs_resolve_r(PyObject *fslike, const vector[string]& parts) except * with gil:
    path = (<object> fslike).resolve_r(parts)
    if path is not None:
        return Path_cpp(PyObj(<PyObject*>path.fsobj), path.parts)
    else:
        return Path_cpp()


cdef Path_cpp fs_resolve_w(PyObject *fslike, const vector[string]& parts) except * with gil:
    path = (<object> fslike).resolve_w(parts)
    if path is not None:
        return Path_cpp(PyObj(<PyObject*>path.fsobj), path.parts)
    else:
        return Path_cpp()


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


cdef bool fs_is_fslike_directory(PyObject *fslike) except * with gil:
    return isinstance(<object> fslike, Directory)


def setup():
    pyx_fs_is_file.bind0(fs_is_file)
    pyx_fs_is_dir.bind0(fs_is_dir)
    pyx_fs_writable.bind0(fs_writable)
    pyx_fs_list.bind0(fs_list)
    pyx_fs_mkdirs.bind0(fs_mkdirs)
    pyx_fs_open_r.bind0(fs_open_r)
    pyx_fs_open_w.bind0(fs_open_w)
    pyx_fs_open_rw.bind0(fs_open_rw)
    pyx_fs_open_a.bind0(fs_open_a)
    pyx_fs_open_ar.bind0(fs_open_ar)
    pyx_fs_resolve_r.bind0(fs_resolve_r)
    pyx_fs_resolve_w.bind0(fs_resolve_w)
    pyx_fs_get_native_path.bind0(fs_get_native_path)
    pyx_fs_rename.bind0(fs_rename)
    pyx_fs_rmdir.bind0(fs_rmdir)
    pyx_fs_touch.bind0(fs_touch)
    pyx_fs_unlink.bind0(fs_unlink)
    pyx_fs_get_mtime.bind0(fs_get_mtime)
    pyx_fs_get_filesize.bind0(fs_get_filesize)
    pyx_fs_is_fslike_directory.bind0(fs_is_fslike_directory)

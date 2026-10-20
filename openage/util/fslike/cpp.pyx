# Copyright 2017-2021 the openage authors. See copying.md for legal info.

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

    def open_r(self, subpath):
        cdef File_cpp file = self.fsobj.get().open_r(subpath)
        return None

    def open_w(self, subpath):
        cdef File_cpp file = self.fsobj.get().open_w(subpath)
        return None

    def resolve_r(self, subpath):
        cdef pair[bool, Path] result = self.fsobj.get().resolve_r(subpath)

        if not result.first:
            return None
        else:
            return cpppath_to_pypath(result.second)

    def resolve_w(self, subpath):
        cdef pair[bool, Path] result = self.fsobj.get().resolve_w(subpath)

        if not result.first:
            return None
        else:
            return cpppath_to_pypath(result.second)

    def get_native_path(self, subpath):
        cdef string native_path = self.fsobj.get().get_native_path(subpath)
        txt = bytes(native_path)

        if txt:
            return txt
        else:
            return None

    def list(self, subpath):
        cdef Path.subpath_t result = self.fsobj.get().list(subpath)

        for entry in result:
            yield from str(entry)

    def filesize(self, subpath):
        return self.fsobj.get().get_filesize(subpath)

    def mtime(self, subpath):
        return self.fsobj.get().get_mtime(subpath)

    def mkdirs(self, subpath):
        return self.fsobj.get().mkdirs(subpath)

    def rmdir(self, subpath):
        return self.fsobj.get().rmdir(subpath)

    def unlink(self, subpath):
        return self.fsobj.get().unlink(subpath)

    def touch(self, subpath):
        self.fsobj.get().touch(subpath)

    def rename(self, srcsubpath, tgtsubpath):
        self.fsobj.get().rename(srcsubpath, tgtsubpath)

    def is_file(self, subpath):
        return self.fsobj.get().is_file(subpath)

    def is_dir(self, subpath):
        return self.fsobj.get().is_dir(subpath)

    def writable(self, subpath):
        return self.fsobj.get().writable(subpath)


class FSLikeCPP(FSLikeObject):
    """
    Pure python cpp fslike wrapper.
    Wrapps the above translation class again so
    we can inherit from FSLikeObject.
    """

    def __init__(self, cpp_wrapper):
        self.fsobj = cpp_wrapper

    def open_r(self, subpath):
        return self.fsobj.open_r(subpath)

    def open_w(self, subpath):
        return self.fsobj.open_w(subpath)

    def resolve_r(self, subpath):
        return self.fsobj.resolve_r(subpath)

    def resolve_w(self, subpath):
        return self.fsobj.resolve_w(subpath)

    def get_native_path(self, subpath):
        return self.fsobj.get_native_path()

    def list(self, subpath):
        yield from self.fsobj.list(subpath)

    def filesize(self, subpath):
        return self.fsobj.get_filesize(subpath)

    def mtime(self, subpath):
        return self.fsobj.get_mtime(subpath)

    def mkdirs(self, subpath):
        return self.fsobj.mkdirs(subpath)

    def rmdir(self, subpath):
        return self.fsobj.rmdir(subpath)

    def unlink(self, subpath):
        return self.fsobj.unlink(subpath)

    def touch(self, subpath):
        self.fsobj.touch(subpath)

    def rename(self, srcsubpath, tgtsubpath):
        self.fsobj.rename(srcsubpath, tgtsubpath)

    def is_file(self, subpath):
        return self.fsobj.is_file(subpath)

    def is_dir(self, subpath):
        return self.fsobj.is_dir(subpath)

    def writable(self, subpath):
        return self.fsobj.is_writable(subpath)


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
        return Path_py(
            path.get_subpath(),
            <object>py_fslike.get_py_fsobj().get_ref(),
        )

    else:
        # wrap cpp fslike to relay calls
        # then pack it into the python path
        return Path_py(
            [],
            FSLikeCPP(
                FSLikeCPPWrapper.wrap(fsobj.shared_from_this()),
            )
        )


cdef bool fs_is_file(PyObject *fslike,
                     const vector[string]& subpath) except * with gil:
    return (<object> fslike).is_file(subpath)


cdef bool fs_is_dir(PyObject *fslike,
                    const vector[string]& subpath) except * with gil:
    return (<object> fslike).is_dir(subpath)


cdef bool fs_writable(PyObject *fslike,
                      const vector[string]& subpath) except * with gil:
    return (<object> fslike).writable(subpath)


cdef vector[string] fs_list(PyObject *fslike, const vector[string]& subpath) except * with gil:
    return (<object> fslike).list(subpath)


cdef bool fs_mkdirs(PyObject *fslike, const vector[string]& subpath) except * with gil:
    return (<object> fslike).mkdirs(subpath)


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
            raise ValueError(f"unknown file open mode id: {mode}")

        # open it the python-way and wrap it
        filelike = path.open(access_mode)
        ref = PyObj(<PyObject*> filelike)
        return File_cpp(ref)


cdef check_file_exists(object path, object fslike, const vector[string]& subpath):
    if path is None:
        raise FileNotFoundError("file could not be found in filesystem %s "
                                "for path '%s'" % (
                                    fslike,
                                    b"/".join(subpath).decode(errors='ignore')
                                ))


cdef File_cpp fs_open_r(PyObject *fslike, const vector[string]& subpath) except * with gil:
    open_path = (<object> fslike).resolve_r(subpath)
    check_file_exists(open_path, <object> fslike, subpath)
    return fs_open(open_path, 0)


cdef File_cpp fs_open_w(PyObject *fslike, const vector[string]& subpath) except * with gil:
    open_path = (<object> fslike).resolve_w(subpath)
    check_file_exists(open_path, <object> fslike, subpath)
    return fs_open(open_path, 1)


cdef File_cpp fs_open_rw(PyObject *fslike, const vector[string]& subpath) except * with gil:
    open_path = (<object> fslike).resolve_w(subpath)
    check_file_exists(open_path, <object> fslike, subpath)
    return fs_open(open_path, 2)


cdef File_cpp fs_open_a(PyObject *fslike, const vector[string]& subpath) except * with gil:
    open_path = (<object> fslike).resolve_w(subpath)
    check_file_exists(open_path, <object> fslike, subpath)
    return fs_open(open_path, 3)


cdef File_cpp fs_open_ar(PyObject *fslike, const vector[string]& subpath) except * with gil:
    open_path = (<object> fslike).resolve_w(subpath)
    check_file_exists(open_path, <object> fslike, subpath)
    return fs_open(open_path, 4)


cdef Path_cpp fs_resolve_r(PyObject *fslike, const vector[string]& subpath) except * with gil:
    path = (<object> fslike).resolve_r(subpath)
    if path is not None:
        return Path_cpp(path.subpath, PyObj(<PyObject*>path.fsobj))
    else:
        return Path_cpp()


cdef Path_cpp fs_resolve_w(PyObject *fslike, const vector[string]& subpath) except * with gil:
    path = (<object> fslike).resolve_w(subpath)
    if path is not None:
        return Path_cpp(path.subpath, PyObj(<PyObject*>path.fsobj))
    else:
        return Path_cpp()


cdef PyObj fs_get_native_path(PyObject *fslike,
                              const vector[string]& subpath) except * with gil:

    path = (<object> fslike).get_native_path(subpath)
    return PyObj(<PyObject*>path)


cdef bool fs_rename(PyObject *fslike,
                    const vector[string]& subpath,
                    const vector[string]& target_subpath) except * with gil:

    return (<object> fslike).rename(subpath, target_subpath)


cdef bool fs_rmdir(PyObject *fslike, const vector[string]& subpath) except * with gil:
    return (<object> fslike).rmdir(subpath)


cdef bool fs_touch(PyObject *fslike, const vector[string]& subpath) except * with gil:
    return (<object> fslike).touch(subpath)


cdef bool fs_unlink(PyObject *fslike, const vector[string]& subpath) except * with gil:
    return (<object> fslike).unlink(subpath)


cdef int fs_get_mtime(PyObject *fslike, const vector[string]& subpath) except * with gil:
    return (<object> fslike).mtime(subpath)


cdef uint64_t fs_get_filesize(PyObject *fslike, const vector[string]& subpath) except * with gil:
    return (<object> fslike).filesize(subpath)


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

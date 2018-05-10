# Copyright 2017-2017 the openage authors. See copying.md for legal info.

"""
Functions called from C++ to perform method calls on
filelike python objects.
"""

import os

from cpython.ref cimport PyObject
from libc.string cimport memcpy
from libcpp cimport bool
from libcpp.memory cimport shared_ptr
from libcpp.string cimport string

from libopenage.util.file cimport File as File_cpp
from libopenage.util.filelike.filelike cimport (
    FileLike,
    seek_t,
    seek_t_SET,
    seek_t_CUR,
    seek_t_END
)
from libopenage.util.filelike.python cimport (
    Python as FileLikePython,
    pyx_file_read,
    pyx_file_read_to,
    pyx_file_readable,
    pyx_file_write,
    pyx_file_writable,
    pyx_file_seek,
    pyx_file_seekable,
    pyx_file_tell,
    pyx_file_close,
    pyx_file_flush,
    pyx_file_size,
)
from .abstract import FileLikeObject


cdef class FileLikeCPPWrapper:
    """
    Wraps a C++ filelike object so it can be used
    from python.
    """

    # pointer to the cpp filelike object
    cdef shared_ptr[FileLike] filelike

    @staticmethod
    cdef wrap(shared_ptr[FileLike] c_fileobj):
        wrp = FileLikeCPPWrapper()
        wrp.filelike = c_fileobj
        return wrp

    def read(self, size=-1):
        return self.filelike.get().read(size)

    def readable(self):
        return self.filelike.get().readable()

    def write(self, data):
        self.filelike.get().write(data)

    def writable(self):
        return self.filelike.get().readable()

    def seek(self, offset, whence=os.SEEK_SET):
        cdef seek_t whence_c

        if whence == os.SEEK_SET:
            whence_c = seek_t_SET
        elif whence == os.SEEK_CUR:
            whence_c = seek_t_CUR
        elif whence == os.SEEK_END:
            whence_c = seek_t_END
        else:
            raise ValueError("invalid whence: %s" % whence)

        self.filelike.get().seek(offset, whence_c)

    def seekable(self):
        return self.filelike.get().seekable()

    def tell(self):
        return self.filelike.get().tell()

    def close(self):
        self.filelike.get().close()

    def flush(self):
        self.filelike.get().flush()

    def get_size(self):
        return self.filelike.get().get_size()


class FileLikeCPP(FileLikeObject):
    """
    Wraps the C++ wrapper again so it can inherit from
    FileLikeObject.
    Relays all calls to the C++ wrapper.
    """

    def __init__(self, cpp_wrapper):
        self.fileobj = cpp_wrapper

    def read(self, size=-1):
        return self.fileobj.read(size)

    def readable(self):
        return self.fileobj.readable()

    def write(self, data):
        self.fileobj.write(data)

    def writable(self):
        return self.fileobj.readable()

    def seek(self, offset, whence=os.SEEK_SET):
        self.fileobj.seek(offset, whence)

    def seekable(self):
        return self.fileobj.seekable()

    def tell(self):
        return self.fileobj.tell()

    def close(self):
        self.fileobj.close()

    def flush(self):
        self.fileobj.flush()

    def get_size(self):
        return self.fileobj.get_size()


cdef cppfile_to_pyfile(const File_cpp &file):
    cdef shared_ptr[FileLike] fileobj = file.get_fileobj();
    cdef FileLikePython *py_filelike

    # we could also use typeid() here

    # if the fileobject is from python, we can use it without wrapping
    if fileobj.get().is_python_native():
        # extract the python filelike object and use it directly
        py_filelike = <FileLikePython *> fileobj.get()
        return (<object> py_filelike.get_py_fileobj().get_ref())

    else:
        # wrap cpp filelike object for python (twice..):
        # first wrap contains the c++ pointer, second allows
        # python inheritance.
        return FileLikeCPP(FileLikeCPPWrapper.wrap(fileobj))


cdef string file_read(PyObject *filelike, ssize_t max) except * with gil:
    return (<object> filelike).read(max)


cdef size_t file_read_to(PyObject *filelike, void *buf, ssize_t max) except * with gil:
    cdef bytes data = (<object> filelike).read(max)
    cdef size_t count = len(data)
    memcpy(buf, <const char *>data, count)
    return count


cdef bool file_readable(PyObject *filelike) except * with gil:
    return (<object> filelike).readable()


cdef void file_write(PyObject *filelike, const string &data) except * with gil:
    (<object> filelike).write(data)


cdef bool file_writable(PyObject *filelike) except * with gil:
    return (<object> filelike).writable()


cdef void file_seek(PyObject *filelike, ssize_t offset, int how) except * with gil:
    # how is SEEK_POS=0, SEEK_CUR=1, SEEK_END=2
    (<object> filelike).seek(offset, how)


cdef bool file_seekable(PyObject *filelike) except * with gil:
    return (<object> filelike).seekable()


cdef size_t file_tell(PyObject *filelike) except * with gil:
    return (<object> filelike).tell()


cdef void file_close(PyObject *filelike) except * with gil:
    (<object> filelike).close()


cdef void file_flush(PyObject *filelike) except * with gil:
    (<object> filelike).flush()


cdef ssize_t file_size(PyObject *filelike) except * with gil:
    return (<object> filelike).size()


def setup():
    pyx_file_read.bind0(file_read)
    pyx_file_read_to.bind0(file_read_to)
    pyx_file_readable.bind0(file_readable)
    pyx_file_write.bind0(file_write)
    pyx_file_writable.bind0(file_writable)
    pyx_file_seek.bind0(file_seek)
    pyx_file_seekable.bind0(file_seekable)
    pyx_file_tell.bind0(file_tell)
    pyx_file_close.bind0(file_close)
    pyx_file_flush.bind0(file_flush)
    pyx_file_size.bind0(file_size)

# Copyright 2017-2017 the openage authors. See copying.md for legal info.

"""
Functions called from C++ to perform method calls on
filelike python objects.
"""


from cpython.ref cimport PyObject
from libcpp cimport bool
from libcpp.string cimport string


from libopenage.util.filelike.python cimport (
    pyx_file_read,
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


cdef string file_read(PyObject *filelike, ssize_t max) except * with gil:
    return (<object> filelike).read(max)


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
    pyx_file_readable.bind0(file_readable)
    pyx_file_write.bind0(file_write)
    pyx_file_writable.bind0(file_writable)
    pyx_file_seek.bind0(file_seek)
    pyx_file_seekable.bind0(file_seekable)
    pyx_file_tell.bind0(file_tell)
    pyx_file_close.bind0(file_close)
    pyx_file_flush.bind0(file_flush)
    pyx_file_size.bind0(file_size)

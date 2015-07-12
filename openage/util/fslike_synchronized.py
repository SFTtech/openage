# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Provides Synchronizer, a thread-safety wrapper.
"""

import os
from threading import Lock

from .fslikeabstract import FileSystemLikeObject
from .filelike import FileLikeObject


class Synchronizer(FileSystemLikeObject):
    """
    The constructor takes the to-be-wrapped FileSystemLikeObject as an
    argument; all calls are performed in a thread-safe manner; any
    File-like objects that are returned by open() use the same locking
    mechanism.
    """

    # pylint: disable=too-few-public-methods

    def __init__(self, obj):
        self.obj = obj
        self.lock = Lock()

    def _listfiles_impl(self, pathcomponents):
        with self.lock:
            return self.obj.listfiles(pathcomponents)

    def _listdirs_impl(self, pathcomponents):
        with self.lock:
            return self.obj.listdirs(pathcomponents)

    def _isfile_impl(self, pathcomponents):
        with self.lock:
            return self.obj.isfile(pathcomponents)

    def _isdir_impl(self, pathcomponents):
        with self.lock:
            return self.obj.isdir(pathcomponents)

    def _open_impl(self, pathcomponents, mode):
        with self.lock:
            result = self.obj.open(pathcomponents, mode)

        return FileSynchronizer(self.lock, result)

    def _mkdirs_impl(self, pathcomponents):
        with self.lock:
            return self.obj.mkdirs(pathcomponents)

    def _remove_impl(self, pathcomponents):
        with self.lock:
            return self.obj.remove(pathcomponents)

    def _rmdir_impl(self, pathcomponents):
        with self.lock:
            return self.obj.rmdir(pathcomponents)

    def _writable_impl(self, pathcomponents):
        with self.lock:
            return self.obj.writable(pathcomponents)

    def _mtime_impl(self, pathcomponents):
        with self.lock:
            return self.obj.mtime(pathcomponents)

    def _filesize_impl(self, pathcomponents):
        with self.lock:
            return self.obj.filesize(pathcomponents)

    def _watch_impl(self, pathcomponents, callback):
        with self.lock:
            return self.obj.watch(pathcomponents, callback)


class FileSynchronizer(FileLikeObject):
    """
    Used by Synchronizer; returned File-like objects are wrapped by this class.

    All method calls are protected by the given lock.
    """
    def __init__(self, lock, obj):
        super().__init__()
        self.lock = lock
        self.obj = obj

    def read(self, size=-1):
        with self.lock:
            return self.obj.read(size)

    def readable(self):
        with self.lock:
            return self.obj.readable()

    def write(self, data):
        with self.lock:
            return self.obj.write(data)

    def writable(self):
        with self.lock:
            return self.obj.writable()

    def seek(self, offset, whence=os.SEEK_SET):
        with self.lock:
            return self.obj.seek(offset, whence)

    def seekable(self):
        with self.lock:
            return self.obj.seekable()

    def tell(self):
        with self.lock:
            return self.obj.tell()

    def close(self):
        with self.lock:
            return self.obj.close()

    def flush(self):
        with self.lock:
            return self.obj.flush()

    def get_size(self):
        with self.lock:
            return self.obj.get_size()

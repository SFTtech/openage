# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Provides

 - Wrapper, a utility class for implementing wrappers around FSLikeObject.
 - WriteBlocker, a wrapper that blocks all writing.
 - Synchronizer, which adds thread-safety to a FSLikeObject.
"""

import os
from threading import Lock

from ..context import DummyGuard
from ..filelike import FileLikeObject

from .abstract import FSLikeObject, ReadOnlyFSLikeObject
from .path import Path


class Wrapper(FSLikeObject):
    """
    Wraps a Path, implementing all methods as pass-through.

    Inherit to override individual methods.
    Pass a context guard to protect calls.
    """
    def __init__(self, obj, contextguard=None):
        if not isinstance(obj, Path):
            raise TypeError("Path expected as obj, got '%s'" % type(obj))

        self.obj = obj
        if contextguard is None:
            self.contextguard = DummyGuard()
        else:
            self.contextguard = contextguard

    def __repr__(self):
        if self.contextguard == DummyGuard:
            return "{}({})".format(type(self).__name__, repr(self.obj))

        return "{}({}, {})".format(
            type(self).__name__, repr(self.obj), repr(self.contextguard))

    def open_r(self, parts):
        with self.contextguard:
            fileobj = self.obj.joinpath(parts).open_r()

        if self.contextguard == DummyGuard:
            return fileobj

        return GuardedFile(fileobj, self.contextguard)

    def open_w(self, parts):
        with self.contextguard:
            fileobj = self.obj.joinpath(parts).open_w()

        if self.contextguard == DummyGuard:
            return fileobj

        return GuardedFile(fileobj, self.contextguard)

    def resolve_r(self, parts):
        return Path(self.obj, parts) if self.is_file(parts) else None

    def resolve_w(self, parts):
        return Path(self.obj, parts) if self.writable(parts) else None

    def list(self, parts):
        with self.contextguard:
            return list(self.obj.joinpath(parts).list())

    def filesize(self, parts):
        with self.contextguard:
            return self.obj.joinpath(parts).filesize

    def mtime(self, parts):
        with self.contextguard:
            return self.obj.joinpath(parts).mtime

    def mkdirs(self, parts):
        with self.contextguard:
            return self.obj.joinpath(parts).mkdirs()

    def rmdir(self, parts):
        with self.contextguard:
            return self.obj.joinpath(parts).rmdir()

    def unlink(self, parts):
        with self.contextguard:
            return self.obj.joinpath(parts).unlink()

    def touch(self, parts):
        with self.contextguard:
            return self.obj.joinpath(parts).touch()

    def rename(self, srcparts, tgtparts):
        with self.contextguard:
            return self.obj.joinpath(srcparts).rename(
                self.obj.joinpath(tgtparts))

    def is_file(self, parts):
        with self.contextguard:
            return self.obj.joinpath(parts).is_file()

    def is_dir(self, parts):
        with self.contextguard:
            return self.obj.joinpath(parts).is_dir()

    def writable(self, parts):
        with self.contextguard:
            return self.obj.joinpath(parts).writable()

    def watch(self, parts, callback):
        with self.contextguard:
            return self.obj.joinpath(parts).watch(callback)

    def poll_watches(self):
        with self.contextguard:
            return self.obj.poll_watches()


class WriteBlocker(ReadOnlyFSLikeObject, Wrapper):
    """
    Wraps a FSLikeObject, transparently passing through all read-only calls.

    All writing calls raise IOError, and writable returns False.
    """
    def __repr__(self):
        return "WriteBlocker({})".format(repr(self.obj))


class Synchronizer(Wrapper):
    """
    Wraps a FSLikeObject, securing all wrapped calls with a mutex.
    """
    def __init__(self, obj):
        self.lock = Lock()
        super().__init__(obj, self.lock)

    def __repr__(self):
        with self.lock:  # pylint: disable=not-context-manager
            return "Synchronizer({})".format(repr(self.obj))


class GuardedFile(FileLikeObject):
    """
    Wraps file-like objects, protecting calls to their members with the given
    context guard.
    """
    def __init__(self, obj, guard):
        super().__init__()
        self.obj = obj
        self.guard = guard

    def read(self, size=-1):
        with self.guard:
            return self.obj.read(size)

    def readable(self):
        with self.guard:
            return self.obj.readable()

    def write(self, data):
        with self.guard:
            return self.obj.write(data)

    def writable(self):
        with self.guard:
            return self.obj.writable()

    def seek(self, offset, whence=os.SEEK_SET):
        with self.guard:
            return self.obj.seek(offset, whence)

    def seekable(self):
        with self.guard:
            return self.obj.seekable()

    def tell(self):
        with self.guard:
            return self.obj.tell()

    def close(self):
        with self.guard:
            return self.obj.close()

    def flush(self):
        with self.guard:
            return self.obj.flush()

    def get_size(self):
        with self.guard:
            return self.obj.get_size()

    def __repr__(self):
        with self.guard:
            return "GuardedFile({}, {})".format(
                repr(self.obj), repr(self.guard))

# Copyright 2015-2022 the openage authors. See copying.md for legal info.

"""
Provides

 - Wrapper, a utility class for implementing wrappers around FSLikeObject.
 - WriteBlocker, a wrapper that blocks all writing.
 - Synchronizer, which adds thread-safety to a FSLikeObject
                 by wrapping a threading.Lock.
 - DirectoryCreator, a wrapper that transparently creates nonexisting
                     directories.
"""

import os
from threading import Lock
from typing import IO

from ..context import DummyGuard
from ..filelike.abstract import FileLikeObject
from .abstract import FSLikeObject, ReadOnlyFSLikeObject, Subpath
from .path import Path


class Wrapper(FSLikeObject):
    """
    Wraps a Path, implementing all methods as pass-through.

    Inherit to override individual methods.
    Pass a context guard to protect calls.
    """

    def __init__(self, path: Path, contextguard=None):
        if not isinstance(path, Path):
            raise TypeError(f"Path expected as obj, got '{type(path)}'")

        self.obj = path
        if contextguard is None:
            self.contextguard = DummyGuard()
        else:
            self.contextguard = contextguard

    def __repr__(self):
        if isinstance(self.contextguard, DummyGuard):
            return f"{type(self).__name__}({self.obj!r})"

        return f"{type(self).__name__}({self.obj!r}, {self.contextguard!r})"

    def _open(self, subpath: Subpath, mode: str) -> IO[bytes] | FileLikeObject:
        with self.contextguard:
            fileobj = self.obj.joinpath(subpath).open(mode)

        if isinstance(self.contextguard, DummyGuard):
            return fileobj

        return GuardedFile(fileobj, self.contextguard)

    def open_r(self, subpath: Subpath) -> IO[bytes] | FileLikeObject:
        return self._open(subpath, "rb")

    def open_w(self, subpath: Subpath) -> IO[bytes] | FileLikeObject:
        return self._open(subpath, "wb")

    def open_rw(self, subpath: Subpath) -> IO[bytes] | FileLikeObject:
        return self._open(subpath, "r+b")

    def open_a(self, subpath: Subpath) -> IO[bytes] | FileLikeObject:
        return self._open(subpath, "ab")

    def open_ar(self, subpath: Subpath) -> IO[bytes] | FileLikeObject:
        return self._open(subpath, "a+b")

    def resolve_r(self, subpath: Subpath):
        return self.obj.joinpath(subpath) if self.exists(subpath) else None

    def resolve_w(self, subpath: Subpath):
        return self.obj.joinpath(subpath) if self.writable(subpath) else None

    def get_native_path(self, subpath: Subpath):
        return self.obj.joinpath(subpath).resolve_native_path() if self.exists(subpath) else None

    def list(self, subpath: Subpath):
        with self.contextguard:
            return list(self.obj.joinpath(subpath).list())

    def filesize(self, subpath: Subpath) -> int:
        with self.contextguard:
            return self.obj.joinpath(subpath).filesize

    def mtime(self, subpath: Subpath) -> float | None:
        with self.contextguard:
            return self.obj.joinpath(subpath).mtime

    def mkdirs(self, subpath: Subpath) -> None:
        with self.contextguard:
            return self.obj.joinpath(subpath).mkdirs()

    def rmdir(self, subpath: Subpath) -> None:
        with self.contextguard:
            return self.obj.joinpath(subpath).rmdir()

    def unlink(self, subpath: Subpath) -> None:
        with self.contextguard:
            return self.obj.joinpath(subpath).unlink()

    def touch(self, subpath: Subpath) -> None:
        with self.contextguard:
            return self.obj.joinpath(subpath).touch()

    def rename(self, srcsubpath: Subpath, tgtsubpath: Subpath) -> None:
        with self.contextguard:
            return self.obj.joinpath(srcsubpath).rename(self.obj.joinpath(tgtsubpath))

    def is_file(self, subpath: Subpath) -> bool:
        with self.contextguard:
            return self.obj.joinpath(subpath).is_file()

    def is_dir(self, subpath: Subpath) -> bool:
        with self.contextguard:
            return self.obj.joinpath(subpath).is_dir()

    def writable(self, subpath: Subpath) -> bool:
        with self.contextguard:
            return self.obj.joinpath(subpath).writable()

    def watch(self, subpath: Subpath, callback) -> bool:
        with self.contextguard:
            return self.obj.joinpath(subpath).watch(callback)

    def poll_watches(self) -> None:
        with self.contextguard:
            return self.obj.poll_fs_watches()


class WriteBlocker(ReadOnlyFSLikeObject, Wrapper):
    """
    Wraps a FSLikeObject, transparently passing through all read-only calls.

    All writing calls raise IOError, and writable returns False.
    """

    def __repr__(self):
        return f"WriteBlocker({self.obj!r})"


class Synchronizer(Wrapper):
    """
    Wraps a FSLikeObject, securing all wrapped calls with a mutex.
    """

    def __init__(self, obj):
        self.lock = Lock()
        super().__init__(obj, self.lock)

    def __repr__(self):
        # TODO: remove override once pylint is fixed.
        with self.lock:  # pylint: disable=not-context-manager
            return f"Synchronizer({self.obj!r})"


class GuardedFile(FileLikeObject):
    """
    Wraps file-like objects, protecting calls to their members with the given
    context guard.
    """

    def __init__(self, obj: FileLikeObject, guard):
        super().__init__()
        self.obj = obj
        self.guard = guard

    @property
    def name(self):
        with self.guard:
            return self.obj.name

    def read(self, size: int = -1):
        with self.guard:
            return self.obj.read(size)

    def readable(self) -> bool:
        with self.guard:
            return self.obj.readable()

    def write(self, data) -> None:
        with self.guard:
            return self.obj.write(data)

    def writable(self) -> bool:
        with self.guard:
            return self.obj.writable()

    def seek(self, offset: int, whence=os.SEEK_SET) -> None:
        with self.guard:
            return self.obj.seek(offset, whence)

    def seekable(self) -> bool:
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

    def get_size(self) -> int:
        with self.guard:
            return self.obj.get_size()

    def truncate(self, size: int) -> int:
        with self.guard:
            return self.obj.truncate(size)

    def fileno(self) -> int:
        with self.guard:
            return self.obj.fileno()

    def __repr__(self):
        with self.guard:
            return f"GuardedFile({self.obj!r}, {self.guard!r})"


class DirectoryCreator(Wrapper):
    """
    Wrapper around a filesystem-like object that automatically creates
    directories when attempting to create a file.
    """

    def open_w(self, subpath):
        self.mkdirs(subpath[:-1])
        return super().open_w(subpath)

    def __repr__(self):
        return f"DirectoryCreator({self.obj})"

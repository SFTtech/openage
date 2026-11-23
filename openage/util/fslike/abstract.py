# Copyright 2015-2022 the openage authors. See copying.md for legal info.

"""
Provides filesystem-like interfaces:

 - FileSystemLikeObject (abstract class)
    an abstract class for objects that represent file systems.

 - ReadOnlyFileSystemLikeObject (abstract class)
    implements all write access functions to raise UnsupportedOperation

For interface implementations, see the fslike module.
"""

from __future__ import annotations

from abc import ABC, abstractmethod
from io import UnsupportedOperation
from typing import IO, TYPE_CHECKING, Generator, List, NoReturn

from .path import Path, Subpath

if TYPE_CHECKING:
    from openage.util.filelike.abstract import FileLikeObject


class FSLikeObject(ABC):
    """
    To be implemented by any filesystem-like objects that wish to provide
    their contents via Path-like and file-like objects.

    Normally, you use the Path provided by self.root for example!

    The abstract member methods take a list or tuple of path component
    bytes objects, e.g.: [b'etc', b'passwd'].

    If a request can not be fulfilled for whatever reason (file doesn't exist,
    the object is read-only, the given method is not implemented, ...),
    they may and shall raise an appropriate instance of IOError.
    """

    @property
    def root(self) -> Path:
        """
        Returns a path-like object for the root of this file system.

        This is the main interface that is used normally.
        """
        return Path([], self)

    def _split_subpath(self, subpath: Subpath) -> List[bytes]:
        """
        Splits the given subpath into a list of bytes components.
        """
        return [part if isinstance(part, bytes) else part.encode() for part in subpath]

    def pretty(self, subpath: Subpath) -> str:
        """
        pretty-format a path in this filesystem like object.
        """
        return f"[{self!s}]:{'/'.join(subpath)}"

    @abstractmethod
    def open_r(self, subpath: Subpath) -> IO[bytes] | FileLikeObject:
        """Shall return a binary reader for the given file ("mode 'rb'")."""

    @abstractmethod
    def open_w(self, subpath: Subpath) -> IO[bytes] | FileLikeObject:
        """Shall return a binary writer for the given file ("mode 'wb'")."""

    @abstractmethod
    def open_rw(self, subpath: Subpath) -> IO[bytes] | FileLikeObject:
        """Shall return a binary random-access handle for the given file ("mode 'r+'")."""

    @abstractmethod
    def open_a(self, subpath: Subpath) -> IO[bytes] | FileLikeObject:
        """Shall return a binary append handle for the given file ("mode 'a'")."""

    @abstractmethod
    def open_ar(self, subpath: Subpath) -> IO[bytes] | FileLikeObject:
        """Shall return a binary random-access append handle for the given file ("mode 'a+'")."""

    def exists(self, subpath: Subpath) -> bool:
        """Test if the subpath is a file or a directory"""
        return self.is_file(subpath) or self.is_dir(subpath)

    def resolve_r(self, subpath: Subpath) -> Path | None:
        """
        Returns a new, flattened, Path if the target exists.
        The fslike subpath in between may be skipped,
        so that just the resulting path is returned.

        Returns None if the path does not exist.
        """
        return Path(path=subpath, fsobj=self) if self.exists(subpath) else None

    def resolve_w(self, subpath: Subpath) -> Path | None:
        """
        Returns a new flattened path. This skips funny mounts in between.

        Returns None if the path does not exist or is not writable.
        """
        return Path(path=subpath, fsobj=self) if self.writable(subpath) else None

    def get_native_path(self, subpath: Subpath) -> str | None:
        """
        Return the path bytestring that represents a location usable
        by your kernel.
        If the path can't be represented natively, return None.
        """
        # By default, return None. It's overridden by subclasses.
        return None

    @abstractmethod
    def list(self, subpath: Subpath) -> Generator[str, None, None]:
        """Shall yield the entry names of the given directory."""

    @abstractmethod
    def filesize(self, subpath: Subpath) -> int:
        """
        Shall determine the file size (bytes),
        and return None if unknown.
        """

    @abstractmethod
    def mtime(self, subpath: Subpath) -> float | None:
        """
        Shall determine the last modification time (UNIX timestamp),
        and return None if unknown.
        """

    @abstractmethod
    def mkdirs(self, subpath: Subpath) -> None:
        """Shall ensure that the directory exists."""

    @abstractmethod
    def rmdir(self, subpath: Subpath) -> None:
        """Shall remove an empty directory."""

    @abstractmethod
    def unlink(self, subpath: Subpath) -> None:
        """Shall remove a single file."""

    @abstractmethod
    def touch(self, subpath: Subpath) -> None:
        """Shall create the file or update its timestamp."""

    @abstractmethod
    def rename(self, srcsubpath: Subpath, tgtsubpath: Subpath) -> None:
        """Shall rename a file or directory to the target name."""

    @abstractmethod
    def is_file(self, subpath: Subpath) -> bool:
        """
        Shall return true if the path is a file (or symlink to one).
        Shall not raise.
        """

    @abstractmethod
    def is_dir(self, subpath: Subpath) -> bool:
        """
        Shall return true if the path is a directory (or symlink to one).
        Shall not raise.
        """

    @abstractmethod
    def writable(self, subpath: Subpath) -> bool:
        """
        Shall return an educated guess whether the path can be written to.
        Shall not raise.
        """

    @abstractmethod
    def watch(self, subpath: Subpath, callback) -> bool:
        """
        Shall install callback as a watcher for the given path, if supported.
        Shall return True if a watcher was installed, False if no operation was
        performed.
        Shall not raise.
        """

    @abstractmethod
    def poll_watches(self) -> None:
        """
        Shall poll all file watches and invoke the associated callbacks if
        any of the files have changed. Shall have a low performance impact.
        Shall not raise.
        """


class ReadOnlyFSLikeObject(FSLikeObject):
    """
    Specialization of FSLikeObject where all writing methods are implemented to
    raise IOError.
    """

    def read_only_error(self, subpath: Subpath) -> NoReturn:
        """Helper method to be called from all other methods."""
        del subpath  # unused
        raise UnsupportedOperation("read-only: " + str(self))

    def open_w(self, subpath: Subpath) -> NoReturn:
        self.read_only_error(subpath)

    def open_a(self, subpath: Subpath) -> NoReturn:
        self.read_only_error(subpath)

    def open_ar(self, subpath: Subpath) -> NoReturn:
        self.read_only_error(subpath)

    def open_rw(self, subpath: Subpath) -> NoReturn:
        self.read_only_error(subpath)

    def mkdirs(self, subpath: Subpath) -> NoReturn:
        self.read_only_error(subpath)

    def rmdir(self, subpath: Subpath) -> NoReturn:
        self.read_only_error(subpath)

    def unlink(self, subpath: Subpath) -> NoReturn:
        self.read_only_error(subpath)

    def touch(self, subpath: Subpath) -> NoReturn:
        self.read_only_error(subpath)

    def rename(self, srcsubpath: Subpath, tgtsubpath: Subpath) -> NoReturn:
        del tgtsubpath  # unused
        self.read_only_error(srcsubpath)

    def writable(self, subpath: Subpath) -> bool:
        del subpath  # unused
        return False

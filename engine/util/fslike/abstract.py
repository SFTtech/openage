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
import typing

from abc import ABC, abstractmethod
from io import UnsupportedOperation

from .path import Path

if typing.TYPE_CHECKING:
    from io import BufferedReader


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

    # sorry pylint, we need those methods.
    # pylint: disable=too-many-public-methods

    @property
    def root(self):
        """
        Returns a path-like object for the root of this file system.

        This is the main interface that is used normally.
        """
        return Path(self, [])

    def pretty(self, parts) -> str:
        """
        pretty-format a path in this filesystem like object.
        """
        return f"[{str(self)}]:{b'/'.join(parts).decode(errors='replace')}"

    @abstractmethod
    def open_r(self, parts) -> BufferedReader:
        """ Shall return a BufferedReader for the given file ("mode 'rb'"). """

    @abstractmethod
    def open_w(self, parts) -> BufferedReader:
        """ Shall return a BufferedWriter for the given file ("mode 'wb'"). """

    def open_rw(self, parts) -> BufferedReader:
        """ Shall return a BufferedWriter for the given file ("mode 'r+'"). """

    def open_a(self, parts) -> BufferedReader:
        """ Shall return a BufferedWriter for the given file ("mode 'a'"). """

    def open_ar(self, parts) -> BufferedReader:
        """ Shall return a BufferedWriter for the given file ("mode 'a+'"). """

    def exists(self, parts):
        """ Test if the parts are a file or a directory """
        return self.is_file(parts) or self.is_dir(parts)

    def resolve_r(self, parts) -> typing.Union[Path, None]:
        """
        Returns a new, flattened, Path if the target exists.
        The fslike parts in between may be skipped,
        so that just the resulting path is returned.

        Returns None if the path does not exist.
        """
        return Path(self, parts) if self.exists(parts) else None

    def resolve_w(self, parts) -> typing.Union[Path, None]:
        """
        Returns a new flattened path. This skips funny mounts in between.

        Returns None if the path does not exist or is not writable.
        """
        return Path(self, parts) if self.writable(parts) else None

    def get_native_path(self, parts) -> typing.ByteString:  # pylint: disable=no-self-use,unused-argument,useless-return
        """
        Return the path bytestring that represents a location usable
        by your kernel.
        If the path can't be represented natively, return None.
        """
        # By default, return None. It's overridden by subclasses.
        return None

    @abstractmethod
    def list(self, parts) -> typing.Generator[str | bytes, None, None]:
        """ Shall yield the entry names of the given directory. """

    @abstractmethod
    def filesize(self, parts) -> int:
        """
        Shall determine the file size (bytes),
        and return None if unknown.
        """

    @abstractmethod
    def mtime(self, parts) -> typing.Union[float, None]:
        """
        Shall determine the last modification time (UNIX timestamp),
        and return None if unknown.
        """

    @abstractmethod
    def mkdirs(self, parts) -> None:
        """ Shall ensure that the directory exists. """

    @abstractmethod
    def rmdir(self, parts) -> None:
        """ Shall remove an empty directory. """

    @abstractmethod
    def unlink(self, parts) -> None:
        """ Shall remove a single file. """

    @abstractmethod
    def touch(self, parts) -> None:
        """ Shall create the file or update its timestamp. """

    @abstractmethod
    def rename(self, srcparts, tgtparts) -> None:
        """ Shall rename a file or directory to the target name. """

    @abstractmethod
    def is_file(self, parts) -> bool:
        """
        Shall return true if the path is a file (or symlink to one).
        Shall not raise.
        """

    @abstractmethod
    def is_dir(self, parts) -> bool:
        """
        Shall return true if the path is a directory (or symlink to one).
        Shall not raise.
        """

    @abstractmethod
    def writable(self, parts) -> bool:
        """
        Shall return an educated guess whether the path can be written to.
        Shall not raise.
        """

    @abstractmethod
    def watch(self, parts, callback) -> bool:
        """
        Shall install callback as a watcher for the given path, if supported.
        Shall return True if a watcher was installed, False if no operation was
        performed.
        Shall not raise.
        """

    @abstractmethod
    def poll_watches(self):
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
    # pylint doesn't understand that this class is supposed to be abstract.
    # pylint: disable=abstract-method

    def read_only_error(self, parts) -> typing.NoReturn:
        """ Helper method to be called from all other methods. """
        del parts  # unused
        raise UnsupportedOperation("read-only: " + str(self))

    def open_w(self, parts) -> typing.NoReturn:
        self.read_only_error(parts)

    def mkdirs(self, parts) -> typing.NoReturn:
        self.read_only_error(parts)

    def rmdir(self, parts) -> typing.NoReturn:
        self.read_only_error(parts)

    def unlink(self, parts) -> typing.NoReturn:
        self.read_only_error(parts)

    def touch(self, parts) -> typing.NoReturn:
        self.read_only_error(parts)

    def rename(self, srcparts, tgtparts) -> typing.NoReturn:
        del tgtparts  # unused
        self.read_only_error(srcparts)

    def writable(self, parts) -> bool:
        del parts  # unused
        return False

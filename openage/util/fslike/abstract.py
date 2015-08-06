# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Provides filesystem-like interfaces:

 - FileSystemLikeObject (abstract class)
    an abstract class for objects that represent file systems.

 - ReadOnlyFileSystemLikeObject (abstract class)
    implements all write access functions to raise UnsupportedOPeration

For interface implementations, see the fslike module.
"""

from abc import ABC, abstractmethod
from io import UnsupportedOperation

from .path import Path


class FSLikeObject(ABC):
    """
    To be implemented by any filesystem-like objects that wish to provide
    their contents via Path-like and file-like objects.

    The abstract member methods take a list or tuple of path component
    bytes objects, e.g.: [b'etc', b'passwd'].

    If a request can not be fulfilled for whatever reason (file doesn't exist,
    the object is read-only, the given method is not implemented, ...),
    they may and shall raise an appropriate instance of IOError.
    """
    @property
    def root(self):
        """ Returns a path-like object for the root of this file system. """
        return Path(self, [])

    @abstractmethod
    def open_r(self, parts):
        """ Shall return a BufferedReader for the given file ("mode 'rb'"). """
        pass

    @abstractmethod
    def open_w(self, parts):
        """ Shall return a BufferedWriter for the given file ("mode 'wb'"). """
        pass

    @abstractmethod
    def list(self, parts):
        """ Shall yield the entry names of the given directory. """

    @abstractmethod
    def filesize(self, parts):
        """
        Shall determine the file size (bytes),
        and return None if unknown.
        """
        pass

    @abstractmethod
    def mtime(self, parts):
        """
        Shall determine the last modification time (UNIX timestamp),
        and return None if unknown.
        """
        pass

    @abstractmethod
    def mkdirs(self, parts):
        """ Shall ensure that the directory exists. """
        pass

    @abstractmethod
    def rmdir(self, parts):
        """ Shall remove an empty directory. """
        pass

    @abstractmethod
    def unlink(self, parts):
        """ Shall remove a single file. """
        pass

    @abstractmethod
    def touch(self, parts):
        """ Shall create the file or update its timestamp. """
        pass

    @abstractmethod
    def rename(self, srcparts, tgtparts):
        """ Shall rename a file or directory to the target name. """
        pass

    @abstractmethod
    def is_file(self, parts):
        """
        Shall return true if the path is a file (or symlink to one).
        Shall not raise.
        """
        pass

    @abstractmethod
    def is_dir(self, parts):
        """
        Shall return true if the path is a directory (or symlink to one).
        Shall not raise.
        """
        pass

    @abstractmethod
    def writable(self, parts):
        """
        Shall return an educated guess whether the path can be written to.
        Shall not raise.
        """
        pass

    @abstractmethod
    def watch(self, parts, callback):
        """
        Shall install callback as a watcher for the given path, if supported.
        Shall return True if a watcher was installed, False if no operation was
        performed.
        Shall not raise.
        """
        pass

    @abstractmethod
    def poll_watches(self):
        """
        Shall poll all file watches and invoke the associated callbacks if
        any of the files have changed. Shall have a low performance impact.
        Shall not raise.
        """
        pass


class ReadOnlyFSLikeObject(FSLikeObject):
    """
    Specialization of FSLikeObject where all writing methods are implemented to
    raise IOError.
    """
    # pylint doesn't understand that this class is supposed to be abstract.
    # pylint: disable=abstract-method

    def read_only_error(self, parts):
        """ Helper method to be called from all other methods. """
        del parts  # unused
        raise UnsupportedOperation("read-only: " + str(self))

    def open_w(self, parts):
        self.read_only_error(parts)

    def mkdirs(self, parts):
        self.read_only_error(parts)

    def rmdir(self, parts):
        self.read_only_error(parts)

    def unlink(self, parts):
        self.read_only_error(parts)

    def touch(self, parts):
        self.read_only_error(parts)

    def rename(self, srcparts, tgtparts):
        del tgtparts  # unused
        self.read_only_error(srcparts)

    def writable(self, parts):
        del parts  # unused
        return False

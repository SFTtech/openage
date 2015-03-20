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

from .math import INF


def parse_pathname(pathname):
    """
    Converts a pathname, as accepted by the FSLikeObject interface methods,
    to a 'pathcomponents' list of strings, as accepted by the impl methods.

    Path objects are either str objects or byte objects that contain
    a '/'-separted pathname.

    Pathnames starting with '/' are interpreted as if starting with './'.

    '..' and '.' are removed during sanitization.
    """
    if isinstance(pathname, bytes):
        pathname = pathname.decode()

    if isinstance(pathname, str):
        pathname = pathname.split('/')

    result = []
    for component in pathname:
        if component == '' or component == '.':
            continue
        elif component == '..':
            if not result:
                raise ValueError(
                    "Path " + repr(pathname) + " "
                    "points at superdirectory of root")

            result.pop()
        else:
            result.append(component)

    return result


class FileSystemLikeObject(ABC):
    """
    Abstract base class for objects that represent file systems.

    Offers a consistent interface for all sorts of data sources,
    such as archive files, or actual directories.

    Various member functions are specified in this interface,
    most importantly listfiles(), which returns a iterable file list,
    and open(), which returns a file-like object.
    """
    def listfiles(self, pathname='.'):
        """
        Lists all file names in the given pathname.

        Returns an iterable of strings.
        """
        pathcomponents = parse_pathname(pathname)
        return self._listfiles_impl(pathcomponents)

    @abstractmethod
    def _listfiles_impl(self, pathcomponents):
        """ Implementation of listfiles; takes path components. """
        pass

    def listdirs(self, pathname='.'):
        """
        Lists all subdir names in the given pathname.

        Does not return '.' or '..'.

        Returns an iterable of strings.
        """
        pathcomponents = parse_pathname(pathname)
        return self._listdirs_impl(pathcomponents)

    @abstractmethod
    def _listdirs_impl(self, pathcomponents):
        """ Implementation of listdirs; takes path components. """
        pass

    def isfile(self, pathname):
        """
        Returns true if there is a file with the given pathname.
        """
        pathcomponents = parse_pathname(pathname)
        return self._isfile_impl(pathcomponents)

    @abstractmethod
    def _isfile_impl(self, pathcomponents):
        """ Implementation of isfile; takes path components. """
        pass

    def isdir(self, pathname):
        """
        Returns true if there as a directory with the given pathname.
        """
        pathcomponents = parse_pathname(pathname)
        return self._isdir_impl(pathcomponents)

    @abstractmethod
    def _isdir_impl(self, pathcomponents):
        """ Implementation of isdir; takes path components. """
        pass

    def open(self, pathname, mode='r'):
        """
        returns a file-like object for the given file path, and mode.

        only a subset of modes may be implemented.

        read-only implementations shall raise UnsupportedOperation if
        writing or file creation is requested.
        """
        pathcomponents = parse_pathname(pathname)
        return self._open_impl(pathcomponents, mode)

    @abstractmethod
    def _open_impl(self, pathcomponents, mode):
        """ Implementation of open; takes path components. """
        pass

    def mkdirs(self, pathname):
        """
        Creates a directory with the given pathname (including parents).

        Read-only implementations shall raise UnsupportedOperation.
        """
        pathcomponents = parse_pathname(pathname)
        self._mkdirs_impl(pathcomponents)

    @abstractmethod
    def _mkdirs_impl(self, pathcomponents):
        """ Implementation of mkdirs; takes path components. """
        pass

    def remove(self, pathname):
        """
        Deletes the file (but not the directory) at the given pathname.

        Read-only implementations shall raise UnsupportedOperation.
        """
        pathcomponents = parse_pathname(pathname)
        self._remove_impl(pathcomponents)

    @abstractmethod
    def _remove_impl(self, pathcomponents):
        """ Implementation of remove; takes path components. """
        pass

    def rmdir(self, pathname):
        """
        Deletes the (empty) directory at the given pathname.

        Read-only implementations shall raise UnsupportedOperation.
        """
        pathcomponents = parse_pathname(pathname)
        self._rmdir_impl(pathcomponents)

    @abstractmethod
    def _rmdir_impl(self, pathcomponents):
        """ Implementation of rmdir; takes path components. """
        pass

    def removerecursive(self, pathname):
        """
        (Recursively) deletes the file or directory at the given pathname.

        Read-only implementations shall raise UnsupportedOperation.
        """
        if self.isfile(pathname):
            self.remove(pathname)

        elif self.isdir(pathname):
            for filename in self.listfiles(pathname):
                self.remove(pathname + '/' + filename)
            for dirname in self.listdirs(pathname):
                self.removerecursive(pathname + '/' + dirname)

            self.rmdir(pathname)

        else:
            raise FileNotFoundError(
                "File or directory does not exist: " + str(pathname))

    def writable(self, pathname='.'):
        """
        Checks if the given path can (probably) be written to.

        Returns a boolean value.

        Pathname may be non-existin.

        Read-only implementations shall always return False.
        """
        pathcomponents = parse_pathname(pathname)
        return self._writable_impl(pathcomponents)

    @abstractmethod
    def _writable_impl(self, pathcomponents):
        """ Implementation of writable; takes path components. """
        pass

    def mtime(self, pathname):
        """
        Returns the date of last file modification.

        UnsupportedOperation may be raised for directories, or for all calls.
        """
        pathcomponents = parse_pathname(pathname)
        return self._mtime_impl(pathcomponents)

    @abstractmethod
    def _mtime_impl(self, pathcomponents):
        """ Implementation of mtime; takes path components. """
        pass

    def filesize(self, pathname):
        """
        Returns the size of the file at the given pathname, in bytes.
        """
        pathcomponents = parse_pathname(pathname)
        return self._filesize_impl(pathcomponents)

    @abstractmethod
    def _filesize_impl(self, pathcomponents):
        """ Implementation of filesize; takes path components. """
        pass

    def watch(self, pathname, callback):
        """
        Installs a watcher that monitors pathname for changes.

        The callback is invoked on every modification.

        Non-supporting implementations shall simply treat this as a no-op.
        """
        pathcomponents = parse_pathname(pathname)
        return self._watch_impl(pathcomponents, callback)

    @abstractmethod
    def _watch_impl(self, pathcomponents, callback):
        """
        Implementation of watch; takes path components and the
        to-be-installed callback.
        """
        pass

    def print_tree(self, path="", prefix="", max_entries=INF):
        """
        Invokes fsprinting.print_tree to pretty-print the directory tree.
        """
        from .fsprinting import print_tree

        print_tree(self, path, prefix, max_entries)


class ReadOnlyFileSystemLikeObject(FileSystemLikeObject):
    """
    Abstract method designed for use by read-only file system-like objects.

    Implements the UnsupportedOperation recommendations.
    """

    # pylint doesn't understand that this class is supposed to be abstract.
    # pylint: disable=abstract-method

    def _mkdirs_impl(self, pathcomponents):
        raise UnsupportedOperation(
            "can not create directory " + str(pathcomponents) + " "
            "in read-only object")

    def removerecursive(self, pathname):
        raise UnsupportedOperation(
            "can not recursively remove " + str(pathname))

    def _remove_impl(self, pathcomponents):
        raise UnsupportedOperation(
            "can not remove file " + str(pathcomponents) + " "
            "in read-only object")

    def _rmdir_impl(self, pathcomponents):
        raise UnsupportedOperation(
            "can not remove directory " + str(pathcomponents) + " "
            "in read-only object")

    def _writable_impl(self, pathcomponents):
        del pathcomponents  # unused

        return False

    def open(self, pathname, mode='r'):
        """
        Verifies that 'mode' is a read-only mode.
        On failure, raises UnsupportedOperation.

        Otherwise, invokes _open_impl as usual.
        """
        pathcomponents = parse_pathname(pathname)

        if 'w' in mode or '+' in mode or 'a' in mode or 'x' in mode:
            raise UnsupportedOperation(
                "can only open " + str(pathname) + " read-only")

        return self._open_impl(pathcomponents, mode)

# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Provides the FileLikeObject abstract base class, which specifies a file-like
interface, and various classes that implement the interface.
"""

from abc import ABC, abstractmethod
from io import UnsupportedOperation
import os


class FileLikeObject(ABC):
    """
    Abstract base class for file-like objects.

    Note that checking isinstance(obj, FileLikeObject) is a bad idea, because
    that would exclude actual files, and Python's built-in file-like objects.

    Does not implement/force implementation of line-reading functionality.
    """
    def __init__(self):
        self.closed = False

    @abstractmethod
    def read(self, size=-1):
        """
        Read at most size bytes (less if EOF has been reached).

        Shall raise UnsupportedOperation for write-only objects.
        """
        pass

    @abstractmethod
    def readable(self):
        """
        Returns True if read() is allowed.
        """
        pass

    @abstractmethod
    def write(self, data):
        """
        Writes all of data to the file.

        Shall raise UnsupportedOperation for read-only object.

        There is no return value.
        """
        pass

    @abstractmethod
    def writable(self):
        """
        Returns True if write() is allowed.
        """
        pass

    @abstractmethod
    def seek(self, offset, whence=os.SEEK_SET):
        """
        Seeks to a given position.

        May raise UnsupportedOperation for any or all arguments, in case of
        unseekable streams.

        For testing seek capabilities, it's recommended to call seek(0)
        immediately after object creation.

        There is no return value.
        """
        pass

    @abstractmethod
    def seekable(self):
        """
        Returns True if seek() is allowed.
        """
        pass

    @abstractmethod
    def tell(self):
        """
        Returns the current position in the file.

        Must work properly for all file-like objects.
        """
        pass

    @abstractmethod
    def close(self):
        """
        Frees internal resources, making the object unusable.
        May be a no-op.
        """
        pass

    @abstractmethod
    def flush(self):
        """
        Syncs data with the disk, or something
        May be a no-op.
        """
        pass

    @abstractmethod
    def get_size(self):
        """
        Returns the size of the object, if known.
        Returns -1 otherwise.

        Note: Actual file objects don't have this method;
              it exists mostly for internal usage.
        """
        pass

    # allow usage with 'with'
    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        del exc_type, exc_val, exc_tb  # unused

        self.close()

    def seek_helper(self, offset, whence):
        """
        Helper function for use by implementations of seek().

        Calculates the new cursor position from offset, whence,
        and self.tell().

        If size is given, it works for whence=os.SEEK_END;
        otherwise, UnsupportedOperation is raised.
        """

        if whence == os.SEEK_SET:
            target = offset
        elif whence == os.SEEK_CUR:
            target = offset + self.tell()
        elif whence == os.SEEK_END:
            size = self.get_size()
            if size < 0:
                raise UnsupportedOperation(
                    "can only seek relative to file start or cursor")
            target = offset + size
        else:
            raise UnsupportedOperation("unsupported seek mode")

        if target < 0:
            raise ValueError("can not seek to a negative file position")

        return target

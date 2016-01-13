# Copyright 2015-2016 the openage authors. See copying.md for legal info.

"""
Provides the FileLikeObject abstract base class, which specifies a file-like
interface, and various classes that implement the interface.
"""

from abc import ABC, abstractmethod
from io import UnsupportedOperation
import os

from .bytequeue import ByteQueue, ByteBuffer
from .math import INF, clamp


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


class ReadOnlyFileLikeObject(FileLikeObject):
    """
    Most FileLikeObjects are read-only, and don't need to implement flush
    or write.

    This abstract class avoids code duplication.
    """

    # pylint doesn't understand that this class is supposed to be abstract.
    # pylint: disable=abstract-method

    def flush(self):
        # no flushing is needed for read-only objects.
        pass

    def readable(self):
        return True

    def write(self, data):
        del data  # unused
        raise UnsupportedOperation("read-only file")

    def writable(self):
        return False


class PosSavingReadOnlyFileLikeObject(ReadOnlyFileLikeObject):
    """
    Stores the current seek position in self.pos.

    Avoids code duplication.
    """

    # pylint doesn't understand that this file is supposed to be abstract.
    # pylint: disable=abstract-method

    def __init__(self):
        super().__init__()
        self.pos = 0

    def seek(self, offset, whence=os.SEEK_SET):
        self.pos = self.seek_helper(offset, whence)

    def seekable(self):
        return True

    def tell(self):
        return self.pos


class StreamSeekBuffer(PosSavingReadOnlyFileLikeObject):
    """
    Wrapper file-like object that adds seek functionality to a read-only,
    unseekable stream.

    For this purpose, all data read from that stream is cached.

    Constructor arguments:

    @param wrappee:
        The non-seekable, read-only stream that is to be wrapped.

    @param keepbuffered:
        If given, only this amount of bytes is guaranteed to stay in the
        seekback buffer.
        If too large of a seekback is requested, an attept at wrappee.reset()
        is made; if that doesn't work, tough luck.

    @param minread:
        If given, read calls to wrappee will request at least this amount
        of bytes (performance optimization).
        By default, entire megabytes are read at once.
    """
    def __init__(self, wrappee, keepbuffered=INF, minread=1048576):
        super().__init__()

        self.wrapped = wrappee
        self.keepbuffered = keepbuffered
        self.minread = minread

        # invariant: len(self.buf) == self.wrapped.tell()
        self.buf = ByteBuffer()

    def resetwrappeed(self):
        """
        resets the wrappeed object, and clears self.buf.
        """
        self.wrapped.reset()
        self.buf = ByteBuffer()

    def read(self, size=-1):
        if size < 0:
            size = INF

        # see if we have already discarded the requested data
        if self.buf.hasbeendiscarded(self.pos):
            # shit.
            # try to reset the stream and start over, hoping that the stream
            # actually implements .reset().
            self.wrapped.reset()
            self.buf = ByteBuffer()

        # see if we need to request some more data from the input stream.
        needed = self.pos + size - len(self.buf)

        while needed > 0:
            # read at most 64 MiB at once
            # read at least self.minread
            amount = clamp(needed, self.minread, 67108864)

            data = self.wrapped.read(amount)
            self.buf.append(data)

            needed -= len(data)

            self.buf.discardleft(max(self.keepbuffered, size - needed))

            if len(data) < amount:
                # wrapped stream has EOFed.
                break

        data = self.buf[self.pos: self.pos + size]
        self.pos += len(data)
        return data

    def get_size(self):
        return self.wrapped.get_size()

    def close(self):
        self.closed = True
        del self.buf
        del self.wrapped


class StreamFragment(PosSavingReadOnlyFileLikeObject):
    """
    Represents a definite part of an other file-like, read-only seekable
    stream.

    Constructor arguments:

    @param stream
        The stream; must implement read(), and seek() with whence=os.SEEK_SET.

        The stream's cursor is explicitly positioned before each call to
        read(); this allows multiple PartialStream objects to use the stream
        in parallel.

    @param start
        The first position of the stream that is used in this object.

    @param size
        The size of the stream fragment (in bytes).
    """

    def __init__(self, stream, start, size):
        super().__init__()

        self.stream = stream
        self.start = start
        self.size = size

        if size < 0:
            raise ValueError("size must be positive")

    def read(self, size=-1):
        if size < 0:
            size = INF

        size = clamp(size, 0, self.size - self.pos)

        if not size:
            return b""

        self.stream.seek(self.start + self.pos)
        data = self.stream.read(size)

        if len(data) != size:
            raise EOFError("unexpected EOF in stream when attempting to read "
                           "stream fragment")

        self.pos += len(data)
        return data

    def get_size(self):
        return self.size

    def close(self):
        self.closed = True
        del self.stream


class FIFO(FileLikeObject):
    """
    File-like wrapper around ByteQueue.

    Data written via write() can later be retrieved via read().

    EOF handling is a bit tricky:

    EOF in the data source (the writer) can not be auto-detected, and must
    be manually indicated by calling seteof().

    Only then will read() show the desired behavior for EOF.

    If EOF has not yet been set, read() will raise ValueError if more data
    than currently available has been requested.
    """

    def __init__(self):
        super().__init__()
        self.eof = False
        self.queue = ByteQueue()
        self.pos = 0

    def tell(self):
        """
        Warning: Returns the position for reading.

        Due to the FIFO nature, the position for writing is further-advanced.
        """
        return self.pos

    def tellw(self):
        """
        Returns the position for writing.
        """
        return self.pos + len(self.queue)

    def seek(self, offset, whence=os.SEEK_SET):
        """
        Unsupported because this is a FIFO.
        """
        del offset, whence  # unused

        raise UnsupportedOperation("unseekable stream")

    def seekable(self):
        return False

    def __len__(self):
        """
        Returns the amount of currently-enqueued data.
        """
        return len(self.queue)

    def seteof(self):
        """
        Declares that no more data will be added using write().

        Note that this does _not_ mean that no more data is available
        through write; the queue may still hold some data.
        """
        self.eof = True

    def write(self, data):
        """
        Works until seteof() has been called; accepts bytes objects.
        """
        if self.eof:
            raise ValueError("EOF has been set; can't write more data")

        self.queue.append(data)

    def writable(self):
        return True

    def read(self, size=-1):
        """
        If seteof() has not been called yet, requesting more data than
        len(self) raises a ValueError.

        When called without arguments, all currently-enqueued data is
        returned; if seteof() has not been set yet, this doesn't
        indicate EOF, though.
        """
        if size < 0:
            size = len(self.queue)
        elif self.eof and size > len(self.queue):
            size = len(self.queue)

        self.pos += size

        return self.queue.popleft(size)

    def readable(self):
        return True

    def get_size(self):
        return len(self.queue)

    def flush(self):
        # no-op
        pass

    def close(self):
        self.closed = True
        self.queue = None

# Copyright 2017-2017 the openage authors. See copying.md for legal info.

"""
Provides a FileLikeObject that acts like a FIFO.
"""

import os
from io import UnsupportedOperation

from .abstract import FileLikeObject
from ..bytequeue import ByteQueue


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

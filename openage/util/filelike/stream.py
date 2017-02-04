# Copyright 2017-2017 the openage authors. See copying.md for legal info.

"""
Provides FileLikeObject for binary stream interaction.
"""

from ..math import INF, clamp

from .readonly import PosSavingReadOnlyFileLikeObject
from ..bytequeue import ByteBuffer


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

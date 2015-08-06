# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
Provides ByteQueue, a high-performance queue for bytes objects.
"""

from collections import deque
from bisect import bisect


class ByteQueue:
    """
    Queue for bytes
    Can append bytes objects at the right,
    and pop arbitrary-size bytes objects at the left.

    The naive implementation would look like this:

    append(self, data):
        self.buf += data

    popleft(self, size):
        result, self.buf = self.buf[:size], self.buf[size:]
        return result

    However, due to python's nature, that would be extremely slow.

    Thus, the bytes objects are actually stored - unmodified -
    in an internal queue, and only concatenated during popleft().
    """

    def __init__(self):
        self.bufs = deque()

        # stores the amount of data that's currently available.
        self.size = 0

    def __len__(self):
        """
        Size of all currently-stored data.
        """
        return self.size

    def append(self, data):
        """
        Adds bytes to the buffer.
        """
        if not isinstance(data, bytes):
            raise TypeError("expected a bytes object, but got " + repr(data))

        self.bufs.append(data)
        self.size += len(data)

    def popleft(self, size):
        """
        Returns the requested amount of bytes from the buffer.
        """
        if size > self.size:
            raise ValueError("ByteQueue does not contain enough bytes")

        self.size -= size

        resultbufs = []
        required = size

        while required > 0:
            buf = self.bufs.popleft()
            resultbufs.append(buf)
            required -= len(buf)

        if required < 0:
            # we requested too much; split the last buffer
            buf = resultbufs.pop()

            popped = buf[:required]
            kept = buf[required:]

            resultbufs.append(popped)
            self.bufs.appendleft(kept)

        return b"".join(resultbufs)


class ByteBuffer:
    """
    Similar to ByteQueue, but instead of popleft, allows reading random slices,
    and trimleft, which discards data from the left.
    """
    def __init__(self):
        # holds all appended bytes objects
        # discarded bytes objects are replaced by None.
        self.bufs = [None]

        # holds the absolute position of the end of each of the byte objects.
        self.index = [0]

        # holds the number of discarded buffers at the left of self.bufs
        # (i.e. the index of the first non-discarded buffer)
        self.discardedbufs = 1

        self.discardedbytes = 0

    def __len__(self):
        return self.index[-1]

    def append(self, data):
        """
        appends new data to the right of the buffer
        """
        if not isinstance(data, bytes):
            raise TypeError("expected bytes, but got " + repr(data))

        self.bufs.append(data)
        self.index.append(len(self) + len(data))

    def discardleft(self, keep):
        """
        discards data at the beginning of the buffer.
        keeps at least the 'keep' most recent bytes.
        """
        discardamount = len(self) - keep
        if discardamount <= self.discardedbytes:
            return

        # the index of the first block that is not to be discarded
        discardto = bisect(self.index, discardamount)

        for idx in range(self.discardedbufs, discardto):
            self.bufs[idx] = None

        self.discardedbufs = discardto
        self.discardedbytes = discardamount

    def hasbeendiscarded(self, position):
        """
        returns True if the given position has already been discarded
        and is no longer valid.
        """
        return position < self.discardedbytes

    def __getitem__(self, pos):
        """
        a slice with default stepping is required.

        when attempting to access already-discarded data, a ValueError
        is raised.
        """

        if not isinstance(pos, slice):
            raise TypeError("expected slice")

        if pos.step is not None:
            raise TypeError("slicing with steps is not supported")

        start, end = pos.start, pos.stop

        # emulate proper slicing behavior
        if start is None:
            start = 0

        if end is None:
            end = len(self)

        if start < 0:
            start = len(self) + start

        if end < 0:
            end = len(self) + end

        return b"".join(self.get_buffers(start, end))

    class DiscardedError(Exception):
        """
        raised by get_buffers and the indexing operator if the requested
        data has already been discarded.
        """
        pass

    def get_buffers(self, start, end):
        """
        yields any amount of bytes objects that constitute the data
        between start and end.

        used internally by __getitem__, but may be useful externally
        as well.

        performs bounds checking, but end must be greater than start.
        """
        # sanitize the start and end values.
        start = max(start, 0)
        end = min(end, len(self))

        if not end > start:
            yield b""
            return

        # check for discard
        if self.hasbeendiscarded(start):
            raise self.DiscardedError(start, end)

        idx = bisect(self.index, start)
        buf = self.bufs[idx]

        # cut off superfluous parts at the left of the first buffer.
        # the negative index is intentional.
        buf = buf[start - self.index[idx]:]

        remaining = end - start
        while remaining > len(buf):
            remaining -= len(buf)
            yield buf

            # get next buffer
            idx += 1
            buf = self.bufs[idx]

        # cut of superfluous parts at the right of the last buffer.
        buf = buf[:remaining]
        yield buf

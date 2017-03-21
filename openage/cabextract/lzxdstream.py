# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
Wraps the LZXDecompressor in a file-like, read-only stream object.
"""

import os
from io import UnsupportedOperation

from ..util.filelike.readonly import ReadOnlyFileLikeObject
from ..util.bytequeue import ByteQueue
from ..util.math import INF

from .lzxd import LZXDecompressor


class LZXDStream(ReadOnlyFileLikeObject):
    """
    Read-only stream object that wraps LZXDecompressor.

    Constructor arguments:

    @param compressed_file
        The compressed file-like object; must implement only read().
        If seek(0) works on it, reset() works on this object.

    @param window_bits
        Provided as metadata in MSCAB files; see LZXDecompressor.

        Defaults to 21.

    @param reset_interval
        Zero for MSCAB files; see LZXDecompressor.

        Theoretically, if reset_interval > 0, efficient seek() could
        be implemented. However, it isn't.

        Defaults to 0.
    """

    def __init__(self, sourcestream, window_bits=21, reset_interval=0):
        super().__init__()

        self.sourcestream = sourcestream
        self.window_bits = window_bits
        self.reset_interval = reset_interval

        # position in the decompressed output stream.
        self.pos = None
        self.buf = None

        self.reset()

    def reset(self):
        """
        Resets the decompressor back to the start of the file.
        """
        self.sourcestream.seek(0)

        self.decompressor = LZXDecompressor(self.sourcestream.read,
                                            self.window_bits,
                                            self.reset_interval)

        self.pos = 0
        self.buf = ByteQueue()

    def read(self, size=-1):
        if size < 0:
            size = INF

        while len(self.buf) < size:
            data = self.decompressor.decompress_next_frame()
            if not data:
                # EOF; return all we have
                return self.buf.popleft(len(self.buf))

            self.buf.append(data)

        return self.buf.popleft(size)

    def get_size(self):
        del self  # unused
        # size is unknown in advance
        return -1

    def seek(self, offset, whence=os.SEEK_SET):
        del offset, whence  # unused
        raise UnsupportedOperation("Cannot seek in LZXDStream.")

    def seekable(self):
        return False

    def tell(self):
        return self.pos

    def close(self):
        self.closed = True
        del self.decompressor
        del self.sourcestream

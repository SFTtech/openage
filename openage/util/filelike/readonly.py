# Copyright 2017-2017 the openage authors. See copying.md for legal info.

"""
Provides an abstract read-only FileLikeObject.
"""

import os
from io import UnsupportedOperation

from .abstract import FileLikeObject


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

# Copyright 2013-2015 the openage authors. See copying.md for legal info.

"""
Code for reading Genie .DRS archives.

Note that .DRS archives can't store file names; they just store the file
extension, and a file number.
"""

from io import UnsupportedOperation
from collections import OrderedDict

from ..log import spam, dbg
from ..util.strings import decode_until_null
from ..util.struct import NamedStruct
from ..util.fslikeabstract import ReadOnlyFileSystemLikeObject
from ..util.filelike import StreamFragment

# version of the drs files, hardcoded for now
FILE_VERSION = 57

if FILE_VERSION == 57:
    COPYRIGHT_SIZE = 40
elif FILE_VERSION == 59:
    COPYRIGHT_SIZE = 60


class DRSHeader(NamedStruct):
    """
    DRS file header; see doc/media/drs-files
    """

    # pylint: disable=bad-whitespace,too-few-public-methods

    endianness       = "<"

    copyright        = str(COPYRIGHT_SIZE) + "s"
    version          = "4s"
    ftype            = "12s"
    table_count      = "i"
    file_offset      = "i"     # offset of the first file


class DRSTableInfo(NamedStruct):
    """
    DRS table header
    """

    # pylint: disable=bad-whitespace,too-few-public-methods

    endianness       = "<"

    file_type        = "B"
    file_extension   = "3s"    # reversed (for reasons) extension
    file_info_offset = "i"     # table offset
    file_count       = "i"     # number of files in table


class DRSFileInfo(NamedStruct):
    """
    DRS file header
    """

    # pylint: disable=bad-whitespace,too-few-public-methods

    endianness       = "<"

    file_id          = "i"
    file_data_offset = "i"
    file_size        = "i"


class DRS(ReadOnlyFileSystemLikeObject):
    """
    represents a file archive in DRS format.
    """
    def __init__(self, fileobj):
        # queried from the outside
        self.fileobj = fileobj

        # read header
        header = DRSHeader.read(fileobj)
        header.copyright = decode_until_null(header.copyright)
        header.version = decode_until_null(header.version)
        header.ftype = decode_until_null(header.ftype)
        self.header = header

        dbg("DRS file: " + str(header))

        # read table info
        self.tables = []
        for i in range(header.table_count):
            table_header = DRSTableInfo.read(fileobj)

            # decode and un-flip the file extension
            fileext = table_header.file_extension
            fileext = fileext.decode('latin-1').lower()[::-1]
            table_header.file_extension = fileext

            spam("Table header %d: %s" % (i, table_header))
            self.tables.append(table_header)

        self.files = OrderedDict()
        for filename, offset, size in self.read_tables():
            self.files[filename] = offset, size

    def read_tables(self):
        """
        Reads the tables from self.tables, and yields tuples of
        filename, offset, size.
        """
        # read file tables
        for header in self.tables:
            self.fileobj.seek(header.file_info_offset)

            for _ in range(header.file_count):
                fileinfo = DRSFileInfo.read(self.fileobj)

                file_name = str(fileinfo.file_id) + '.' + header.file_extension
                spam(file_name + ": " + str(fileinfo))

                yield file_name, fileinfo.file_data_offset, fileinfo.file_size

    def get_file_info(self, pathcomponents):
        """
        For a given path, returns the file metadata from self.files.

        Raises FileNotFoundError, or returns a tuple of offset, size.
        """
        if len(pathcomponents) != 1:
            raise FileNotFoundError('/'.join(pathcomponents))

        filename = pathcomponents[0].lower()

        try:
            return self.files[filename]
        except KeyError:
            raise FileNotFoundError(filename) from None

    def _listfiles_impl(self, pathcomponents):
        if pathcomponents != []:
            raise FileNotFoundError("DRS file has no subdirectories")

        return self.files

    def _listdirs_impl(self, pathcomponents):
        if pathcomponents != []:
            raise FileNotFoundError("DRS file has no subdirectories")

        return []

    def _isfile_impl(self, pathcomponents):
        if len(pathcomponents) != 1:
            return False

        return pathcomponents[0].lower() in self.files

    def _isdir_impl(self, pathcomponents):
        return len(pathcomponents) == 0

    def _open_impl(self, pathcomponents, mode):
        if mode != 'rb':
            raise UnsupportedOperation(
                "open mode for files in DRS archive must be 'rb'")

        offset, size = self.get_file_info(pathcomponents)
        return StreamFragment(self.fileobj, offset, size)

    def _mtime_impl(self, pathcomponents):
        raise UnsupportedOperation("mtime not known in DRS archives")

    def _filesize_impl(self, pathcomponents):
        _, size = self.get_file_info(pathcomponents)
        return size

    def _watch_impl(self, pathcomponents, callback):
        # no actual functionality
        del self, pathcomponents, callback  # unused
        return

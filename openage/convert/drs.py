# Copyright 2013-2018 the openage authors. See copying.md for legal info.

"""
Code for reading Genie .DRS archives.

Note that .DRS archives can't store file names; they just store the file
extension, and a file number.
"""

from ..log import spam, dbg
from ..util.strings import decode_until_null
from ..util.struct import NamedStruct
from ..util.fslike.filecollection import FileCollection
from ..util.filelike.stream import StreamFragment
from openage.convert.dataformat.version_detect import GameEdition

# version of the drs files, hardcoded for now
COPYRIGHT_SIZE_ENSEMBLE = 40
COPYRIGHT_SIZE_LUCAS = 60


class DRSHeaderEnsemble(NamedStruct):
    """
    DRS file header for AoE1 and AoE2; see doc/media/drs-files
    """

    # pylint: disable=bad-whitespace,too-few-public-methods

    endianness       = "<"

    copyright        = str(COPYRIGHT_SIZE_ENSEMBLE) + "s"
    version          = "4s"
    ftype            = "12s"
    table_count      = "i"
    file_offset      = "i"     # offset of the first file


class DRSHeaderLucasArts(NamedStruct):
    """
    DRS file header for SWGB; see doc/media/drs-files
    """

    # pylint: disable=bad-whitespace,too-few-public-methods

    endianness       = "<"

    copyright        = str(COPYRIGHT_SIZE_LUCAS) + "s"
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

    file_extension   = "4s"    # reversed (for reasons) extension
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


class DRS(FileCollection):
    """
    represents a file archive in DRS format.
    """

    def __init__(self, fileobj, game_version):
        super().__init__()

        # queried from the outside
        self.fileobj = fileobj

        # read header
        if GameEdition.SWGB is game_version[0]:
            header = DRSHeaderLucasArts.read(fileobj)

        else:
            header = DRSHeaderEnsemble.read(fileobj)

        header.copyright = decode_until_null(header.copyright).strip()
        header.version = decode_until_null(header.version)
        header.ftype = decode_until_null(header.ftype)
        self.header = header

        dbg(header)

        # read table info
        self.tables = []
        for _ in range(header.table_count):
            table_header = DRSTableInfo.read(fileobj)

            # decode and un-flip the file extension
            # see doc/media/drs-files.md
            fileext = table_header.file_extension
            fileext = fileext.decode('latin-1').lower()[::-1].rstrip()
            table_header.file_extension = fileext

            dbg(table_header)
            self.tables.append(table_header)

        for filename, offset, size in self.read_tables():
            def open_r(offset=offset, size=size):
                """ Returns a opened ('rb') file-like object for fileobj. """
                return StreamFragment(self.fileobj, offset, size)

            self.add_fileentry(
                [filename.encode()],
                (open_r, None, lambda size=size: size, None)
            )

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
                spam("%s: %s", file_name, fileinfo)

                yield file_name, fileinfo.file_data_offset, fileinfo.file_size

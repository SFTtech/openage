# Copyright 2013-2022 the openage authors. See copying.md for legal info.

"""
Code for reading Genie .DRS archives.

Note that .DRS archives can't store file names; they just store the file
extension, and a file number.
"""
from __future__ import annotations
import typing


from .....log import spam, dbg
from .....util.filelike.stream import StreamFragment
from .....util.fslike.filecollection import FileCollection
from .....util.strings import decode_until_null
from .....util.struct import NamedStruct

if typing.TYPE_CHECKING:
    from engine.convert.value_object.init.game_version import GameVersion
    from engine.util.fslike.wrapper import GuardedFile


# version of the drs files, hardcoded for now
COPYRIGHT_ENSEMBLE = b"Copyright (c) 1997 Ensemble Studios"
COPYRIGHT_SIZE_ENSEMBLE = 40
COPYRIGHT_SIZE_LUCAS = 60


class DRSHeaderEnsemble(NamedStruct):
    """
    DRS file header for AoE1 and AoE2; see doc/media/drs-files
    """

    # pylint: disable=too-few-public-methods

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

    # pylint: disable=too-few-public-methods

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

    # pylint: disable=too-few-public-methods

    endianness       = "<"

    file_extension   = "4s"    # reversed (for reasons) extension
    file_info_offset = "i"     # table offset
    file_count       = "i"     # number of files in table


class DRSFileInfo(NamedStruct):
    """
    DRS file header
    """

    # pylint: disable=too-few-public-methods

    endianness       = "<"

    file_id          = "i"
    file_data_offset = "i"
    file_size        = "i"


class DRS(FileCollection):
    """
    represents a file archive in DRS format.
    """

    def __init__(self, fileobj: GuardedFile, game_version: GameVersion):
        super().__init__()

        # queried from the outside
        self.fileobj = fileobj

        # read header
        if game_version.edition.game_id == "SWGB":
            header = DRSHeaderLucasArts.read(fileobj)

        else:
            # Try Ensemble header by default
            header = DRSHeaderEnsemble.read(fileobj)

            if not header.copyright.startswith(COPYRIGHT_ENSEMBLE):
                # different copyright string probably means it's SWGB
                fileobj.seek(0)
                header = DRSHeaderLucasArts.read(fileobj)

        header.copyright = decode_until_null(header.copyright).strip()
        header.version = decode_until_null(header.version)
        header.ftype = decode_until_null(header.ftype)
        self.header = header

        dbg(header)

        # read table info
        self.tables: list[DRSTableInfo] = []
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

    def read_tables(self) -> typing.Generator[tuple[str, str, str], None, None]:
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

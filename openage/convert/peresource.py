# Copyright 2015-2016 the openage authors. See copying.md for legal info.

"""
Provides PEResources, which reads the resource section from a PEFile.
"""

from collections import defaultdict

from ..util.struct import NamedStruct
from ..util.filelike import StreamFragment

from .hardcoded.langcodes import LANGCODES

# types for id in resource directory root node
RESOURCE_TYPES = {
    0: 'unknown',
    1: 'cursor',
    2: 'bitmap',
    3: 'icon',
    4: 'menu',
    5: 'dialog',
    6: 'string',
    7: 'fontdir',
    8: 'font',
    9: 'accelerator',
    10: 'rcdata',
    11: 'messagetable',
    12: 'group_cursor',
    14: 'group_icon',
    16: 'version',
    17: 'dlginclude',
    19: 'plugplay',
    20: 'vxd',
    21: 'anicursor',
    22: 'aniicon',
    23: 'html',
    24: 'manifest'
}

# reverse look-up table
RESOURCE_IDS = {value: key for key, value in RESOURCE_TYPES.items()}

# string resource-specific constants
STRINGTABLE_SIZE = 16


class ResourceDirectory(NamedStruct):
    """
    Resource directory header.
    """

    # pylint: disable=bad-whitespace,too-few-public-methods

    endianness = "<"

    characteristics       = "I"     # some irrelevant flags.
    timestamp             = "I"     # an irrelevant timestamp.
    version_major         = "H"
    version_minor         = "H"

    # the number of named directory entries that follow immediately after.
    named_entry_count     = "H"

    # the number of id directory entries that follow immediately after.
    id_entry_count        = "H"

    subdirs               = None
    leaves                = None


class ResourceDirectoryEntry(NamedStruct):
    """
    these follow immediately after the directory.
    """

    # pylint: disable=bad-whitespace,too-few-public-methods

    endianness = "<"

    # if the high bit is set, the lower 31 bits point to the (utf-16-le) name.
    name                  = "I"

    # if the high bit is set, the lower 31 bits point to an other directory.
    # if the high bit is cleared, they point to a leaf node.
    data                  = "I"

    # set manually later
    name_is_str           = None
    is_subdir             = None


class ResourceLeaf(NamedStruct):
    """
    header for a leaf node in the resource tree.
    """

    # pylint: disable=bad-whitespace,too-few-public-methods

    endianness = "<"

    data_ptr              = "I"
    data_size             = "I"

    codepage              = "I"     # code page id of the data.
    reserved              = "I"     # 0

    # filled in later
    fileobj               = None    # used for open

    def open(self):
        """
        Returns a file-like object for this resource.
        """
        self.fileobj.seek(self.data_ptr)
        return StreamFragment(self.fileobj, self.data_ptr, self.data_size)


class StringLiteral(NamedStruct):
    """
    A simple length-prefixed little-endian utf-16 string.
    """

    # pylint: disable=bad-whitespace,too-few-public-methods

    endianness = "<"

    length             = "H"

    # filled later by read_content()
    value              = None

    @classmethod
    def readall(cls, fileobj):
        """
        In addition to the static data, reads the string.
        """
        result = cls.read(fileobj)
        result.value = fileobj.read(result.length * 2).decode('utf-16-le')
        return result


class PEResources:
    """
    .rsrc section of a PE file

    The constructor takes a PEFile object.
    """
    def __init__(self, pefile):
        self.data, self.datava = pefile.open_section('.rsrc')

        # self.data is at position 0 (i.e. it points at the root directory).
        self.rootdir = self.read_directory()
        self.strings = self.read_strings()

    def __getitem__(self, key):
        return self.rootdir[key]

    def read_directory(self):
        """
        reads the directory that's currently pointed at by self.data.

        descends recursively for subdirectories.

        returns a ResourceDirectory object with both subdirs and leaves filled
        in.
        """
        directory = ResourceDirectory.read(self.data)

        result = {}

        # read directory entries
        entry_count = directory.named_entry_count + directory.id_entry_count
        for idx in range(entry_count):
            entry = ResourceDirectoryEntry.read(self.data)

            # check the entry name
            entry.name_is_str = bool(entry.name & (1 << 31))

            if entry.name_is_str and idx >= directory.named_entry_count:
                raise Exception("expected an id entry, but got a str entry")

            if not entry.name_is_str and idx < directory.named_entry_count:
                raise Exception("expected a str entry, but got an id entry")

            # read the entry name string, if needed
            if entry.name_is_str:
                data_pos = self.data.tell()
                self.data.seek(entry.name - (1 << 31))
                entry.name = StringLiteral.readall(self.data).value
                self.data.seek(data_pos)

            # read the struct pointed at by entry.data
            entry.is_subdir = bool(entry.data & (1 << 31))
            data_pos = self.data.tell()
            if entry.is_subdir:
                # recursively descend
                self.data.seek(entry.data - (1 << 31))
                result[entry.name] = self.read_directory()
            else:
                # read leaf metadata
                self.data.seek(entry.data)
                leaf = ResourceLeaf.read(self.data)
                leaf.data_ptr -= self.datava
                leaf.fileobj = self.data
                result[entry.name] = leaf
            self.data.seek(data_pos)

        return result

    def read_strings(self):
        """
        reads all ressource strings from self.root;
        returns a dict of dicts: {languageid: {stringid: str}}
        """
        result = defaultdict(lambda: {})

        try:
            string_dir = self.rootdir[RESOURCE_IDS['string']]
        except KeyError:
            return result

        # strings are grouped in tables of 16 each.
        for table_id, table_dir in string_dir.items():
            # counting from 1. great idea, Microsoft.
            base_string_id = (table_id - 1) * STRINGTABLE_SIZE

            # each table has leafs for one or more languages
            for lang_id, string_table in table_dir.items():
                langcode = LANGCODES[lang_id]
                string_table_resource = string_table.open()
                for idx in range(STRINGTABLE_SIZE):
                    string = StringLiteral.readall(string_table_resource).value
                    if string:
                        result[langcode][base_string_id + idx] = string

                if sum(string_table_resource.read()) > 0:
                    raise Exception("string table invalid: the padding "
                                    "contains data.")

        return result

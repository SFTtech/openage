# Copyright 2014-2014 the openage authors. See copying.md for legal info.

# handler routines for creating and converting data files

from . import blendomatic
from . import colortable
from . import filelist
from . import stringresource
from . import texture
from .dataformat import multisubtype_base
from .dataformat.data_formatter import DataFormatter
from .gamedata import empiresdat


def generate_gamedata_structs(cpp_src_dir):
    #generate files in these formats
    formats = ("struct", "structimpl")

    data_formatter = DataFormatter()

    struct_data = list()
    struct_data += multisubtype_base.MultisubtypeBaseFile.structs()
    struct_data += empiresdat.EmpiresDat.structs()
    struct_data += blendomatic.Blendomatic.structs()
    struct_data += colortable.ColorTable.structs()
    struct_data += texture.Texture.structs()
    struct_data += filelist.SoundList.structs()
    struct_data += stringresource.StringResource.structs()

    data_formatter.add_data(struct_data)
    output_data = data_formatter.export(formats)

    for file_name, file_data in output_data.items():
        yield file_name, file_data

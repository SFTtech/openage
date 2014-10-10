# handler routines for creating and converting data files

from . import blendomatic
from . import colortable
from . import dataformat
from . import filelist
from .gamedata import empiresdat
from . import stringresource
from . import texture
from .util import dbg, set_write_dir, file_write, file_get_path
from fnmatch import fnmatch


def generate_gamedata_structs(cpp_src_dir):
    #generate files in these formats
    formats = ("struct", "structimpl")

    data_formatter = dataformat.DataFormatter()

    struct_data = list()
    struct_data += dataformat.MultisubtypeMember.MultisubtypeBaseFile.structs()
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

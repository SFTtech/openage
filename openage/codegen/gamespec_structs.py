# Copyright 2015-2015 the openage authors. See copying.md for legal info.

"""
gamespec struct code generation listing.
"""

from ..convert.dataformat.data_formatter import DataFormatter

from ..convert.dataformat.multisubtype_base import MultisubtypeBaseFile
from ..convert.gamedata.empiresdat import EmpiresDat
from ..convert.blendomatic import Blendomatic
from ..convert.colortable import ColorTable
from ..convert.texture import Texture
from ..convert.filelist import SoundList
from ..convert.stringresource import StringResource

from ..util.fslike import SubDirectory


def generate_gamespec_structs(projectdir):
    """ Header and C++ files for the gamespec structs """
    generator = DataFormatter()

    generator.add_data(MultisubtypeBaseFile.structs())
    generator.add_data(EmpiresDat.structs())
    generator.add_data(Blendomatic.structs())
    generator.add_data(ColorTable.structs())
    generator.add_data(Texture.structs())
    generator.add_data(SoundList.structs())
    generator.add_data(StringResource.structs())

    cppdir = SubDirectory(projectdir, 'cpp/gamedata')

    generator.export(cppdir, ("struct", "structimpl"))

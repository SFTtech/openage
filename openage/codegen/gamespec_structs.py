# Copyright 2015-2017 the openage authors. See copying.md for legal info.

"""
gamespec struct code generation listing.
"""

from ..convert.export.data_formatter import DataFormatter

from ..convert.dataformat.multisubtype_base import MultisubtypeBaseFile
from ..convert.gamedata.empiresdat import EmpiresDat
from ..convert.blendomatic import Blendomatic
from ..convert.colortable import ColorTable
from ..convert.texture import Texture
from ..convert.stringresource import StringResource


def generate_gamespec_structs(projectdir):
    """ Header and C++ files for the gamespec structs """
    generator = DataFormatter()

    generator.add_data(MultisubtypeBaseFile.structs())
    generator.add_data(EmpiresDat.structs())
    generator.add_data(Blendomatic.structs())
    generator.add_data(ColorTable.structs())
    generator.add_data(Texture.structs())
    generator.add_data(StringResource.structs())

    cpppath = projectdir.joinpath('libopenage/gamedata')
    generator.export(cpppath, ("struct", "structimpl"))

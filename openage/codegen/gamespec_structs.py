# Copyright 2015-2020 the openage authors. See copying.md for legal info.

"""
gamespec struct code generation listing.
"""

from ..convert.deprecated.data_formatter import DataFormatter
from ..convert.deprecated.multisubtype_base import MultisubtypeBaseFile
from ..convert.entity_object.export.texture import Texture
from ..convert.entity_object.language.stringresource import StringResource
from ..convert.value_object.media.blendomatic import Blendomatic
from ..convert.value_object.media.colortable import ColorTable
from ..convert.value_object.media.datfile.empiresdat import EmpiresDat


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

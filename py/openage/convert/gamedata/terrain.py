# Copyright 2013-2014 the openage authors. See copying.md for legal info.

from .. import dataformat
from struct import Struct, unpack_from
from ..util import dbg, zstr
from ..util import file_get_path, file_write

from .empiresdat import endianness


class TerrainPassGraphic(dataformat.Exportable):

    name_struct_file   = "terrain"
    name_struct        = "terrain_pass_graphic"
    struct_description = None

    data_format = (
        #when this restriction in unit a was selected, can the unit be placed on this terrain id? 0=no, -1=yes
        (dataformat.READ, "buildable", "int32_t"),
        (dataformat.READ, "graphic_id0", "int32_t"),
        (dataformat.READ, "graphic_id1", "int32_t"),
        (dataformat.READ, "replication_amount", "int32_t"),
    )

    def __init__(self, **args):
        super().__init__(**args)


class TerrainRestriction(dataformat.Exportable):
    """
    access policies for units on specific terrain.
    """

    name_struct_file   = "terrain"
    name_struct        = "terrain_restriction"
    struct_description = "løl TODO"

    data_format = (
        #index of each array == terrain id
        #when this restriction was selected, can the terrain be accessed?
        (dataformat.READ, "terrain_accessible", "float[terrain_count]"),
        (dataformat.READ, "terrain_pass_graphics", dataformat.SubdataMember(
            ref_type=TerrainPassGraphic,
            length="terrain_count",
        )),
    )

    def __init__(self, **args):
        super().__init__(**args)


class Terrain(dataformat.Exportable):
    name_struct        = "terrain_type"
    name_struct_file   = "terrain"
    struct_description = "describes a terrain type, like water, ice, etc."

    data_format = (
        (dataformat.READ_UNKNOWN, None,                 "int16_t"),
        (dataformat.READ_UNKNOWN, None,                 "int16_t"),
        (dataformat.READ_EXPORT, "name0",               "char[13]"),
        (dataformat.READ_EXPORT, "name1",               "char[13]"),
        (dataformat.READ_EXPORT, "slp_id",              "int32_t"),
        (dataformat.READ_UNKNOWN, None,                 "int32_t"),
        (dataformat.READ_EXPORT, "sound_id",            "int32_t"),
        (dataformat.READ_EXPORT, "blend_priority",      "int32_t"),    #see doc/media/blendomatic for blending stuff
        (dataformat.READ_EXPORT, "blend_mode",          "int32_t"),
        (dataformat.READ, "color",                      "uint8_t[3]"),
        (dataformat.READ_UNKNOWN, None,                 "uint8_t[5]"),
        (dataformat.READ_UNKNOWN, None,                 "float"),
        (dataformat.READ_UNKNOWN, None,                 "int8_t[18]"),
        (dataformat.READ, "frame_count",                "int16_t"),
        (dataformat.READ, "angle_count",                "int16_t"),
        (dataformat.READ_EXPORT, "terrain_id",          "int16_t"),
        (dataformat.READ, "elevation_graphic",          "int16_t[54]"),
        (dataformat.READ, "terrain_replacement_id",     "int16_t"),
        (dataformat.READ_EXPORT, "terrain_dimensions0", "int16_t"),
        (dataformat.READ_EXPORT, "terrain_dimensions1", "int16_t"),
        (dataformat.READ, "terrain_border_id",          "int8_t[84]"),
        (dataformat.READ, "terrain_unit_id",            "int16_t[30]"),
        (dataformat.READ, "terrain_unit_density",       "int16_t[30]"),
        (dataformat.READ, "terrain_unit_priority",      "int8_t[30]"),  #when placing two terrain units on the same spot, selects which prevails(=1)
        (dataformat.READ, "terrain_units_used_count",   "int16_t"),     #shall we place units implicitly when this terrain is used
    )

    def __init__(self):
        super().__init__()


class FrameData(dataformat.Exportable):
    name_struct        = "frame_data"
    name_struct_file   = ""
    struct_description = "specification of terrain borders."

    data_format = (
        (dataformat.READ, "frame_id", "int16_t"),
        (dataformat.READ, "flag0", "int16_t"),
        (dataformat.READ, "flag1", "int16_t"),
    )


class TerrainBorder(dataformat.Exportable):
    name_struct        = "terrain_border"
    name_struct_file   = "terrain"
    struct_description = "one inter-terraintile border specification."

    data_format = (
        (dataformat.READ, "enabled", "int16_t"),
        (dataformat.READ, "name0", "char[13]"),
        (dataformat.READ, "name1", "char[13]"),
        (dataformat.READ, "ressource_id", "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t"),
        (dataformat.READ, "color", "uint8_t[3]"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t"),
        (dataformat.READ, "frames", dataformat.SubdataMember(
            ref_type=FrameData,
            length=230,
        )),
        (dataformat.READ, "frame_count", "int16_t"),
        (dataformat.READ_UNKNOWN, None, "int16_t"),
        (dataformat.READ_UNKNOWN, None, "int16_t"),
        (dataformat.READ_UNKNOWN, None, "int16_t"),
    )

import dataformat
from struct import Struct, unpack_from
from util import dbg, zstr
from util import file_get_path, file_write

from .empiresdat import endianness


class TerrainPassGraphic(dataformat.Exportable):

    name_struct_file   = "terrain"
    name_struct        = "terrain_pass_graphic"
    struct_description = None

    data_format = (
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
        (dataformat.READ, "terrain_accessible", "float[terrain_count]"),
        (dataformat.READ, "terrain_pass_graphics", dataformat.SubdataMember(
            ref_type=TerrainPassGraphic,
            length="terrain_count",
        )),
    )

    def __init__(self, **args):
        super().__init__(**args)


class TerrainHeaderData(dataformat.Exportable):
    """
    basic terrain specification section.
    """

    name_struct_file   = "terrain"
    name_struct        = "terrain_header_data"
    struct_description = "specifies basic terrain properties"

    data_format = (
        (dataformat.READ, "terrain_restriction_count", "uint16_t"),
        (dataformat.READ, "terrain_count", "uint16_t"),
        (dataformat.READ, "terrain_restriction_offset0", "int32_t[terrain_restriction_count]"),
        (dataformat.READ, "terrain_restriction_offset1", "int32_t[terrain_restriction_count]"),
        (dataformat.READ, "terrain_restrictions", dataformat.SubdataMember(
            ref_type=TerrainRestriction,
            length="terrain_restriction_count",
            passed_args={"terrain_count"},
        )),
    )


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
        (dataformat.READ_EXPORT, "blend_priority",      "int32_t"),
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
        (dataformat.READ, "terrain_unit_priority",      "int8_t[30]"),
        (dataformat.READ, "terrain_units_used_count",   "int16_t"),
    )

    def __init__(self):
        super().__init__()


class TerrainData(dataformat.Exportable):

    name_struct        = "terrain_data"
    name_struct_file   = "gamedata"
    struct_description = "terrain list"

    data_format = (
        (dataformat.NOREAD_EXPORT, "terrain_count", "uint16_t"),
        (dataformat.READ_EXPORT,   "terrains", dataformat.SubdataMember(
            ref_type=Terrain,
            length="terrain_count",
        )),
    )

    def __init__(self, **args):
        super().__init__(**args)


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


class TerrainBorderData(dataformat.Exportable):
    name_struct        = "terrain_border_data"
    name_struct_file   = "terrain"
    struct_description = "specification of terrain borders."

    data_format = (
        (dataformat.READ, "terrain_border", dataformat.SubdataMember(
            ref_type=TerrainBorder,
            length=16,
        )),
        (dataformat.READ_UNKNOWN, "zero", "int8_t[28]"),
        (dataformat.READ, "terrain_count_additional", "uint16_t"),
        (dataformat.READ_UNKNOWN, "terrain_blob", "uint8_t[12722]"),
    )

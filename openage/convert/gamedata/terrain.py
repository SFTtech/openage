# Copyright 2013-2015 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from ..dataformat.exportable import Exportable
from ..dataformat.members import SubdataMember
from ..dataformat.member_access import READ, READ_EXPORT, READ_UNKNOWN


class TerrainPassGraphic(Exportable):

    name_struct_file   = "terrain"
    name_struct        = "terrain_pass_graphic"
    struct_description = None

    data_format = (
        # when this restriction in unit a was selected, can the unit be placed on this terrain id? 0=no, -1=yes
        (READ, "buildable", "int32_t"),
        (READ, "graphic_id0", "int32_t"),
        (READ, "graphic_id1", "int32_t"),
        (READ, "replication_amount", "int32_t"),
    )

    def __init__(self, **args):
        super().__init__(**args)


class TerrainRestriction(Exportable):
    """
    access policies for units on specific terrain.
    """

    name_struct_file   = "terrain"
    name_struct        = "terrain_restriction"
    struct_description = "løl TODO"

    data_format = (
        # index of each array == terrain id
        # when this restriction was selected, can the terrain be accessed?
        (READ, "accessible_dmgmultiplier", "float[terrain_count]"),  # unit interaction_type activates this as damage multiplier
        (READ, "pass_graphics", SubdataMember(
            ref_type=TerrainPassGraphic,
            length="terrain_count",
        )),
    )

    def __init__(self, **args):
        super().__init__(**args)


class Terrain(Exportable):
    name_struct        = "terrain_type"
    name_struct_file   = "terrain"
    struct_description = "describes a terrain type, like water, ice, etc."

    data_format = (
        (READ_UNKNOWN, None,                 "int16_t"),
        (READ_UNKNOWN, None,                 "int16_t"),
        (READ_EXPORT, "name0",               "char[13]"),
        (READ_EXPORT, "name1",               "char[13]"),
        (READ_EXPORT, "slp_id",              "int32_t"),
        (READ_UNKNOWN, None,                 "int32_t"),
        (READ_EXPORT, "sound_id",            "int32_t"),
        (READ_EXPORT, "blend_priority",      "int32_t"),    # see doc/media/blendomatic for blending stuff
        (READ_EXPORT, "blend_mode",          "int32_t"),
        (READ, "color",                      "uint8_t[3]"),
        (READ_UNKNOWN, None,                 "uint8_t[5]"),
        (READ_UNKNOWN, None,                 "float"),
        (READ_UNKNOWN, None,                 "int8_t[18]"),
        (READ, "frame_count",                "int16_t"),
        (READ, "angle_count",                "int16_t"),
        (READ_EXPORT, "terrain_id",          "int16_t"),
        (READ, "elevation_graphic",          "int16_t[54]"),
        (READ, "terrain_replacement_id",     "int16_t"),
        (READ_EXPORT, "terrain_dimensions0", "int16_t"),
        (READ_EXPORT, "terrain_dimensions1", "int16_t"),
        (READ, "terrain_border_id",          "int8_t[84]"),
        (READ, "terrain_unit_id",            "int16_t[30]"),
        (READ, "terrain_unit_density",       "int16_t[30]"),
        (READ, "terrain_unit_priority",      "int8_t[30]"),  # when placing two terrain units on the same spot, selects which prevails(=1)
        (READ, "terrain_units_used_count",   "int16_t"),     # shall we place units implicitly when this terrain is used
    )

    def __init__(self):
        super().__init__()


class FrameData(Exportable):
    name_struct        = "frame_data"
    name_struct_file   = ""
    struct_description = "specification of terrain borders."

    data_format = (
        (READ, "frame_id", "int16_t"),
        (READ, "flag0", "int16_t"),
        (READ, "flag1", "int16_t"),
    )


class TerrainBorder(Exportable):
    name_struct        = "terrain_border"
    name_struct_file   = "terrain"
    struct_description = "one inter-terraintile border specification."

    data_format = (
        (READ, "enabled", "int16_t"),
        (READ, "name0", "char[13]"),
        (READ, "name1", "char[13]"),
        (READ, "resource_id", "int32_t"),
        (READ_UNKNOWN, None, "int32_t"),
        (READ_UNKNOWN, None, "int32_t"),
        (READ, "color", "uint8_t[3]"),
        (READ_UNKNOWN, None, "int8_t"),
        (READ_UNKNOWN, None, "int32_t"),
        (READ_UNKNOWN, None, "int32_t"),
        (READ, "frames", SubdataMember(
            ref_type=FrameData,
            length=230,
        )),
        (READ, "frame_count", "int16_t"),
        (READ_UNKNOWN, None, "int16_t"),
        (READ_UNKNOWN, None, "int16_t"),
        (READ_UNKNOWN, None, "int16_t"),
    )

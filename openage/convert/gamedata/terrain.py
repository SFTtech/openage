# Copyright 2013-2015 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from ..dataformat.exportable import Exportable
from ..dataformat.members import SubdataMember, IncludeMembers
from ..dataformat.member_access import READ, READ_EXPORT, READ_UNKNOWN


class FrameData(Exportable):
    name_struct_file   = "terrain"
    name_struct        = "frame_data"
    struct_description = "specification of terrain frames."

    data_format = (
        (READ_EXPORT, "frame_count", "int16_t"),
        (READ_EXPORT, "angle_count", "int16_t"),
        (READ_EXPORT, "shape_id", "int16_t"),  # frame index
    )


class TerrainPassGraphic(Exportable):
    name_struct_file   = "terrain"
    name_struct        = "terrain_pass_graphic"
    struct_description = None

    data_format = (
        # when this restriction in unit a was selected, can the unit be placed on this terrain id? 0=no, -1=yes
        (READ, "slp_id_exit_tile", "int32_t"),
        (READ, "slp_id_enter_tile", "int32_t"),
        (READ, "slp_id_walk_tile", "int32_t"),
        (READ, "walk_sprite_rate", "float"),
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
        # unit interaction_type activates this as damage multiplier
        # See unit armor terrain restriction;
        # pass-ability: [no: == 0, yes: > 0]
        # build-ability: [<= 0.05 can't build here, > 0.05 can build]
        # damage: [0: damage multiplier is 1, > 0: multiplier = value]
        (READ, "accessible_dmgmultiplier", "float[terrain_count]"),
        (READ, "pass_graphics", SubdataMember(
            ref_type=TerrainPassGraphic,
            length="terrain_count",
        )),
    )

    def __init__(self, **args):
        super().__init__(**args)


class TerrainAnimation(Exportable):
    name_struct        = "terrain_animation"
    name_struct_file   = "terrain"
    struct_description = "describes animation properties of a terrain type"

    data_format = (
        (READ, "is_animated",                "int8_t"),
        (READ, "animation_frame_count",      "int16_t"),        # number of frames to animate
        (READ, "pause_frame_count",          "int16_t"),        # pause n * (frame rate) after last frame draw
        (READ, "interval",                   "float"),         # time between frames
        (READ, "pause_after_loop",           "float"),         # pause time after last frame
        (READ, "frame",                      "int16_t"),       # current frame (including animation and pause frames)
        (READ, "draw_frame",                 "int16_t"),       # current frame id to draw
        (READ, "animate_last",               "float"),         # last time animation frame was changed
        (READ, "frame_changed",              "int8_t"),        # has the drawframe changed since terrain was drawn
        (READ, "drawn",                      "int8_t"),
    )


class Terrain(Exportable):
    name_struct        = "terrain_type"
    name_struct_file   = "terrain"
    struct_description = "describes a terrain type, like water, ice, etc."

    data_format = (
        (READ_EXPORT, "enabled",             "int8_t"),
        (READ_UNKNOWN, None,                 "int8_t"),
        (READ_EXPORT, "name0",               "char[13]"),
        (READ_EXPORT, "name1",               "char[13]"),
        (READ_EXPORT, "slp_id",              "int32_t"),
        (READ_UNKNOWN, None,                 "int32_t"),
        (READ_EXPORT, "sound_id",            "int32_t"),
        (READ_EXPORT, "blend_priority",      "int32_t"),     # see doc/media/blendomatic.md for blending stuff
        (READ_EXPORT, "blend_mode",          "int32_t"),
        (READ_EXPORT, "map_color_hi",        "uint8_t"),     # color of this terrain tile, mainly used in the minimap.
        (READ_EXPORT, "map_color_med",       "uint8_t"),
        (READ_EXPORT, "map_color_low",       "uint8_t"),
        (READ_EXPORT, "map_color_cliff_lt",  "uint8_t"),
        (READ_EXPORT, "map_color_cliff_rt",  "uint8_t"),
        (READ_EXPORT, "passable_terrain",    "int8_t"),
        (READ_EXPORT, "impassable_terrain",  "int8_t"),

        (READ_EXPORT, None, IncludeMembers(cls=TerrainAnimation)),

        (READ_EXPORT, "elevation_graphics", SubdataMember(
            ref_type=FrameData,   # tile Graphics: flat, 2 x 8 elevation, 2 x 1:1; frame Count, animations, shape (frame) index
            length=19,
        )),

        (READ, "terrain_replacement_id",     "int16_t"),     # draw this ground instead (e.g. forrest draws forrest ground)
        (READ_EXPORT, "terrain_dimension0",  "int16_t"),
        (READ_EXPORT, "terrain_dimension1",  "int16_t"),

        (READ, "borders", SubdataMember(
            # probably references to the TerrainBorders, there are 42 terrains in game
            ref_type="int16_t",
            length=42, # TODO: use 100 here for GameVersion.age2_ak
        )),
        (READ, "terrain_unit_id",            "int16_t[30]"),  # place these unit id on the terrain, with prefs from fields below
        (READ, "terrain_unit_density",       "int16_t[30]"),  # how many of the above units to place
        (READ, "terrain_placement_flag",      "int8_t[30]"),  # when placing two terrain units on the same spot, selects which prevails(=1)
        (READ, "terrain_units_used_count",   "int16_t"),      # how many entries of the above lists shall we use to place units implicitly when this terrain is placed
        (READ_UNKNOWN, None,                 "uint16_t"),
    )

    def __init__(self, **args):
        super().__init__(**args)


class TerrainBorder(Exportable):
    name_struct        = "terrain_border"
    name_struct_file   = "terrain"
    struct_description = "one inter-terraintile border specification."

    data_format = (
        (READ, "enabled", "int8_t"),
        (READ_UNKNOWN, None, "int8_t"),
        (READ, "name0", "char[13]"),
        (READ, "name1", "char[13]"),
        (READ, "slp_id", "int32_t"),
        (READ_UNKNOWN, None, "int32_t"),
        (READ, "sound_id", "int32_t"),
        (READ, "color", "uint8_t[3]"),

        (READ_EXPORT, None, IncludeMembers(cls=TerrainAnimation)),

        (READ, "frames", SubdataMember(
            ref_type=FrameData,
            length=19 * 12,  # number of tile types * 12
        )),

        (READ, "draw_tile", "int16_t"),         # always 0
        (READ, "underlay_terrain", "int16_t"),
        (READ, "border_style", "int16_t"),
    )


class TileSize(Exportable):
    name_struct        = "tile_size"
    name_struct_file   = "terrain"
    struct_description = "size definition of one terrain tile."

    data_format = (
        (READ_EXPORT, "width", "int16_t"),
        (READ_EXPORT, "height", "int16_t"),
        (READ_EXPORT, "delta_z", "int16_t"),
    )

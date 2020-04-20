# Copyright 2013-2019 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from ..game_versions import GameVersion
from ..dataformat.genie_structure import GenieStructure
from ..dataformat.read_members import ArrayMember, SubdataMember, IncludeMembers
from ..dataformat.member_access import READ, READ_EXPORT
from ..dataformat.value_members import MemberTypes as StorageType
from openage.convert.dataformat.version_detect import GameExpansion, GameEdition


class FrameData(GenieStructure):
    name_struct_file   = "terrain"
    name_struct        = "frame_data"
    struct_description = "specification of terrain frames."

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_EXPORT, "frame_count", StorageType.INT_MEMBER, "int16_t"),
            (READ_EXPORT, "angle_count", StorageType.INT_MEMBER, "int16_t"),
            (READ_EXPORT, "shape_id", StorageType.ID_MEMBER, "int16_t"),  # frame index
        ]

        return data_format


class TerrainPassGraphic(GenieStructure):
    name_struct_file   = "terrain"
    name_struct        = "terrain_pass_graphic"
    struct_description = None

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            # when this restriction in unit a was selected, can the unit be placed on this terrain id? 0=no, -1=yes
            (READ, "slp_id_exit_tile", StorageType.ID_MEMBER, "int32_t"),
            (READ, "slp_id_enter_tile", StorageType.ID_MEMBER, "int32_t"),
            (READ, "slp_id_walk_tile", StorageType.ID_MEMBER, "int32_t"),
        ]

        if game_version[0] is GameEdition.SWGB:
            data_format.append((READ, "walk_sprite_rate", StorageType.FLOAT_MEMBER, "float"))
        else:
            data_format.append((READ, "replication_amount", StorageType.INT_MEMBER, "int32_t"))

        return data_format


class TerrainRestriction(GenieStructure):
    """
    access policies for units on specific terrain.
    """

    name_struct_file   = "terrain"
    name_struct        = "terrain_restriction"
    struct_description = "løl TODO"

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            # index of each array == terrain id
            # when this restriction was selected, can the terrain be accessed?
            # unit interaction_type activates this as damage multiplier
            # See unit armor terrain restriction;
            # pass-ability: [no: == 0, yes: > 0]
            # build-ability: [<= 0.05 can't build here, > 0.05 can build]
            # damage: [0: damage multiplier is 1, > 0: multiplier = value]
            (READ, "accessible_dmgmultiplier", StorageType.ARRAY_FLOAT, "float[terrain_count]")
        ]

        if game_version[0] is not GameEdition.ROR:
            data_format.append(
                (READ, "pass_graphics", StorageType.ARRAY_CONTAINER, SubdataMember(
                    ref_type=TerrainPassGraphic,
                    length="terrain_count",
                ))
            )

        return data_format


class TerrainAnimation(GenieStructure):
    name_struct        = "terrain_animation"
    name_struct_file   = "terrain"
    struct_description = "describes animation properties of a terrain type"

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            (READ, "is_animated", StorageType.BOOLEAN_MEMBER, "int8_t"),
            # number of frames to animate
            (READ, "animation_frame_count", StorageType.INT_MEMBER, "int16_t"),
            # pause n * (frame rate) after last frame draw
            (READ, "pause_frame_count", StorageType.INT_MEMBER, "int16_t"),
            # time between frames
            (READ, "interval", StorageType.FLOAT_MEMBER, "float"),
            # pause time between frames
            (READ, "pause_between_loops", StorageType.FLOAT_MEMBER, "float"),
            # current frame (including animation and pause frames)
            (READ, "frame", StorageType.INT_MEMBER, "int16_t"),
            # current frame id to draw
            (READ, "draw_frame", StorageType.INT_MEMBER, "int16_t"),
            # last time animation frame was changed
            (READ, "animate_last", StorageType.FLOAT_MEMBER, "float"),
            # has the drawframe changed since terrain was drawn
            (READ, "frame_changed", StorageType.BOOLEAN_MEMBER, "int8_t"),
            (READ, "drawn", StorageType.BOOLEAN_MEMBER, "int8_t")
        ]

        return data_format


class Terrain(GenieStructure):
    name_struct        = "terrain_type"
    name_struct_file   = "terrain"
    struct_description = "describes a terrain type, like water, ice, etc."

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_EXPORT, "enabled", StorageType.BOOLEAN_MEMBER, "int8_t"),
            (READ, "random", StorageType.INT_MEMBER, "int8_t"),
        ]

        if game_version[0] is GameEdition.AOE2DE:
            data_format.extend([
                (READ_EXPORT, "is_water", StorageType.BOOLEAN_MEMBER, "int8_t"),
                (READ_EXPORT, "hide_in_editor", StorageType.BOOLEAN_MEMBER, "int8_t"),
                (READ_EXPORT, "string_id", StorageType.ID_MEMBER, "int32_t"),
                (READ_EXPORT, "internal_name_len_debug", StorageType.INT_MEMBER, "uint16_t"),
                (READ_EXPORT, "internal_name_len", StorageType.INT_MEMBER, "uint16_t"),
                (READ_EXPORT, "internal_name", StorageType.STRING_MEMBER, "char[internal_name_len]"),
                (READ_EXPORT, "filename_len_debug", StorageType.INT_MEMBER, "uint16_t"),
                (READ_EXPORT, "filename_len", StorageType.INT_MEMBER, "uint16_t"),
                (READ_EXPORT, "filename", StorageType.STRING_MEMBER, "char[filename_len]"),
            ])
        elif game_version[0] is GameEdition.SWGB:
            data_format.extend([
                (READ_EXPORT, "internal_name", StorageType.STRING_MEMBER, "char[17]"),
                (READ_EXPORT, "filename", StorageType.STRING_MEMBER, "char[17]"),
            ])
        else:
            data_format.extend([
                (READ_EXPORT, "internal_name", StorageType.STRING_MEMBER, "char[13]"),
                (READ_EXPORT, "filename", StorageType.STRING_MEMBER, "char[13]"),
            ])

        data_format.extend([
            (READ_EXPORT, "slp_id", StorageType.ID_MEMBER, "int32_t"),
            (READ,        "shape_ptr", StorageType.ID_MEMBER, "int32_t"),
            (READ_EXPORT, "sound_id", StorageType.ID_MEMBER, "int32_t"),
        ])

        if game_version[0] is GameEdition.AOE2DE:
            data_format.extend([
                (READ_EXPORT, "wwise_sound_id", StorageType.ID_MEMBER, "uint32_t"),
                (READ_EXPORT, "wwise_stop_sound_id", StorageType.ID_MEMBER, "uint32_t"),
            ])

        if game_version[0] is not GameEdition.ROR:
            data_format.extend([
                # see doc/media/blendomatic.md for blending stuff
                (READ_EXPORT, "blend_priority", StorageType.ID_MEMBER, "int32_t"),
                (READ_EXPORT, "blend_mode", StorageType.ID_MEMBER, "int32_t"),
            ])
            if game_version[0] is GameEdition.AOE2DE:
                data_format.extend([
                    (READ_EXPORT, "overlay_mask_name_len_debug", StorageType.INT_MEMBER, "uint16_t"),
                    (READ_EXPORT, "overlay_mask_name_len", StorageType.INT_MEMBER, "uint16_t"),
                    (READ_EXPORT, "overlay_mask_name", StorageType.STRING_MEMBER, "char[overlay_mask_name_len]"),
                ])

        data_format.extend([
            (READ_EXPORT, "map_color_hi", StorageType.ID_MEMBER, "uint8_t"),       # color of this terrain tile, mainly used in the minimap.
            (READ_EXPORT, "map_color_med", StorageType.ID_MEMBER, "uint8_t"),
            (READ_EXPORT, "map_color_low", StorageType.ID_MEMBER, "uint8_t"),
            (READ_EXPORT, "map_color_cliff_lt", StorageType.ID_MEMBER, "uint8_t"),
            (READ_EXPORT, "map_color_cliff_rt", StorageType.ID_MEMBER, "uint8_t"),
            (READ_EXPORT, "passable_terrain", StorageType.ID_MEMBER, "int8_t"),
            (READ_EXPORT, "impassable_terrain", StorageType.ID_MEMBER, "int8_t"),

            (READ_EXPORT, None, None, IncludeMembers(cls=TerrainAnimation)),

            (READ_EXPORT, "elevation_graphics", StorageType.ARRAY_CONTAINER, SubdataMember(
                ref_type=FrameData,   # tile Graphics: flat, 2 x 8 elevation, 2 x 1:1;
                length=19,
            )),

            (READ, "terrain_replacement_id", StorageType.ID_MEMBER, "int16_t"),     # draw this ground instead (e.g. forrest draws forrest ground)
            (READ_EXPORT, "terrain_to_draw0", StorageType.ID_MEMBER, "int16_t"),
            (READ_EXPORT, "terrain_to_draw1", StorageType.ID_MEMBER, "int16_t"),
        ])

        if game_version[0] is GameEdition.AOE2DE:
            data_format.append(
                (READ, "terrain_unit_masked_density", StorageType.ARRAY_INT, "int16_t[30]")
            )
        elif game_version[0] is GameEdition.SWGB:
            data_format.append(
                (READ, "borders", StorageType.ARRAY_INT, ArrayMember(
                    "int16_t",
                    55
                ))
            )
        elif GameExpansion.AFRI_KING in game_version[1]:
            data_format.append(
                (READ, "borders", StorageType.ARRAY_INT, ArrayMember(
                    "int16_t",
                    100
                ))
            )
        else:
            data_format.append(
                (READ, "borders", StorageType.ARRAY_INT, ArrayMember(
                    "int16_t",
                    42
                ))
            )

        data_format.extend([
            # place these unit id on the terrain, with prefs from fields below
            (READ, "terrain_unit_id", StorageType.ARRAY_ID, "int16_t[30]"),
            # how many of the above units to place
            (READ, "terrain_unit_density", StorageType.ARRAY_INT, "int16_t[30]"),
            # when placing two terrain units on the same spot, selects which prevails(=1)
            (READ, "terrain_placement_flag", StorageType.ARRAY_BOOL, "int8_t[30]"),
            # how many entries of the above lists shall we use to place units implicitly when this terrain is placed
            (READ, "terrain_units_used_count", StorageType.INT_MEMBER, "int16_t"),
        ])

        if game_version[0] is not GameEdition.SWGB:
            data_format.append((READ, "phantom", StorageType.INT_MEMBER, "int16_t"))

        return data_format


class TerrainBorder(GenieStructure):
    name_struct        = "terrain_border"
    name_struct_file   = "terrain"
    struct_description = "one inter-terraintile border specification."

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            (READ, "enabled", StorageType.BOOLEAN_MEMBER, "int8_t"),
            (READ, "random", StorageType.INT_MEMBER, "int8_t"),
            (READ, "internal_name", StorageType.STRING_MEMBER, "char[13]"),
            (READ, "filename", StorageType.STRING_MEMBER, "char[13]"),
            (READ, "slp_id", StorageType.ID_MEMBER, "int32_t"),
            (READ, "shape_ptr", StorageType.ID_MEMBER, "int32_t"),
            (READ, "sound_id", StorageType.ID_MEMBER, "int32_t"),
            (READ, "color", StorageType.ARRAY_ID, "uint8_t[3]"),

            (READ_EXPORT, None, None, IncludeMembers(cls=TerrainAnimation)),

            (READ, "frames", StorageType.ARRAY_CONTAINER, SubdataMember(
                ref_type=FrameData,
                length=19 * 12,  # number of tile types * 12
            )),

            (READ, "draw_tile", StorageType.INT_MEMBER, "int16_t"),         # always 0
            (READ, "underlay_terrain", StorageType.ID_MEMBER, "int16_t"),
            (READ, "border_style", StorageType.INT_MEMBER, "int16_t"),
        ]

        return data_format


class TileSize(GenieStructure):
    name_struct        = "tile_size"
    name_struct_file   = "terrain"
    struct_description = "size definition of one terrain tile."

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_EXPORT, "width", StorageType.INT_MEMBER, "int16_t"),
            (READ_EXPORT, "height", StorageType.INT_MEMBER, "int16_t"),
            (READ_EXPORT, "delta_z", StorageType.INT_MEMBER, "int16_t"),
        ]

        return data_format

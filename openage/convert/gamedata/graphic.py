# Copyright 2013-2019 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from ..dataformat.genie_structure import GenieStructure
from ..dataformat.read_members import SubdataMember, EnumLookupMember
from ..dataformat.member_access import READ, READ_EXPORT
from ..dataformat.value_members import MemberTypes as StorageType


class GraphicDelta(GenieStructure):
    name_struct        = "graphic_delta"
    name_struct_file   = "graphic"
    struct_description = "delta definitions for ingame graphics files."

    data_format = [
        (READ_EXPORT, "graphic_id", StorageType.ID_MEMBER, "int16_t"),
        (READ, "padding_1", StorageType.INT_MEMBER, "int16_t"),
        (READ, "sprite_ptr", StorageType.INT_MEMBER, "int32_t"),
        (READ_EXPORT, "offset_x", StorageType.INT_MEMBER, "int16_t"),
        (READ_EXPORT, "offset_y", StorageType.INT_MEMBER, "int16_t"),
        (READ, "display_angle", StorageType.INT_MEMBER, "int16_t"),
        (READ, "padding_2", StorageType.INT_MEMBER, "int16_t"),
    ]


class SoundProp(GenieStructure):
    name_struct        = "sound_prop"
    name_struct_file   = "graphic"
    struct_description = "sound id and delay definition for graphics sounds."

    data_format = [
        (READ, "sound_delay", StorageType.INT_MEMBER, "int16_t"),
        (READ, "sound_id", StorageType.ID_MEMBER, "int16_t"),
    ]


class GraphicAttackSound(GenieStructure):
    name_struct        = "graphic_attack_sound"
    name_struct_file   = "graphic"
    struct_description = "attack sounds for a given graphics file."

    data_format = [
        (READ, "sound_props", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=SoundProp,
            length=3,
        )),
    ]


class Graphic(GenieStructure):
    name_struct        = "graphic"
    name_struct_file   = name_struct
    struct_description = "metadata for ingame graphics files."

    data_format = []

    # TODO: Enable conversion for SWGB; replace "name"
    # ===========================================================================
    # if (GameVersion.swgb_10 or GameVersion.swgb_cc) in game_versions:
    #     data_format.append((READ_EXPORT, "name", "char[25]"))
    # else:
    #     data_format.append((READ_EXPORT, "name", "char[21]"))
    # ===========================================================================
    data_format.append((READ_EXPORT, "name", StorageType.STRING_MEMBER, "char[21]"))          # internal name: e.g. ARRG2NNE = archery range feudal Age north european

    # TODO: Enable conversion for SWGB; replace "name"
    # ===========================================================================
    # if (GameVersion.swgb_10 or GameVersion.swgb_cc) in game_versions:
    #     data_format.append((READ_EXPORT, "filename", "char[25]"))
    # else:
    #     data_format.append((READ_EXPORT, "filename", "char[13]"))
    # ===========================================================================
    data_format.append((READ_EXPORT, "filename", StorageType.STRING_MEMBER, "char[13]"))

    data_format.extend([
        (READ_EXPORT, "slp_id", StorageType.ID_MEMBER, "int32_t"),             # id of the graphics file in the drs
        (READ, "is_loaded", StorageType.BOOLEAN_MEMBER, "int8_t"),             # unused
        (READ, "old_color_flag", StorageType.BOOLEAN_MEMBER, "int8_t"),        # unused
        (READ_EXPORT, "layer", StorageType.ID_MEMBER, EnumLookupMember(       # originally 40 layers, higher -> drawn on top
            raw_type    = "int8_t",  # -> same layer -> order according to map position.
            type_name   = "graphics_layer",
            lookup_dict = {
                0: "TERRAIN",      # cliff
                5: "SHADOW",       # farm fields as well
                6: "RUBBLE",
                10: "UNIT_LOW",    # constructions, dead units, tree stumps, flowers, paths
                11: "FISH",
                19: "CRATER",      # rugs
                20: "UNIT",        # buildings, units, damage flames, animations (mill)
                21: "BLACKSMITH",  # blacksmith smoke
                22: "BIRD",        # hawk
                30: "PROJECTILE",  # and explosions
            }
        )),
        (READ_EXPORT, "player_color_force_id", StorageType.ID_MEMBER, "int8_t"),    # force given player color
        (READ_EXPORT, "adapt_color", StorageType.INT_MEMBER, "int8_t"),             # playercolor can be changed on sight (like sheep)
        (READ_EXPORT, "transparent_selection", StorageType.INT_MEMBER, "uint8_t"),  # loop animation
        (READ, "coordinates", StorageType.ARRAY_INT, "int16_t[4]"),
        (READ_EXPORT, "delta_count", StorageType.INT_MEMBER, "uint16_t"),
        (READ_EXPORT, "sound_id", StorageType.ID_MEMBER, "int16_t"),
        (READ_EXPORT, "attack_sound_used", StorageType.INT_MEMBER, "uint8_t"),
        (READ_EXPORT, "frame_count", StorageType.INT_MEMBER, "uint16_t"),       # number of frames per angle
        (READ_EXPORT, "angle_count", StorageType.INT_MEMBER, "uint16_t"),       # number of heading angles stored, some of the frames must be mirrored
        (READ, "speed_adjust", StorageType.FLOAT_MEMBER, "float"),              # multiplies the speed of the unit this graphic is applied to
        (READ_EXPORT, "frame_rate", StorageType.FLOAT_MEMBER, "float"),         # frame rate in seconds
        (READ_EXPORT, "replay_delay", StorageType.FLOAT_MEMBER, "float"),       # seconds to wait before current_frame=0 again
        (READ_EXPORT, "sequence_type", StorageType.ID_MEMBER, "int8_t"),
        (READ_EXPORT, "graphic_id", StorageType.ID_MEMBER, "int16_t"),
        (READ_EXPORT, "mirroring_mode", StorageType.ID_MEMBER, "int8_t"),
    ])

    # TODO: Enable conversion for AOE1; replace "editor_flag"
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.append((READ, "editor_flag", "int8_t"))
    # ===========================================================================
    data_format.append((READ, "editor_flag", StorageType.INT_MEMBER, "int8_t"))  # sprite editor thing for AoK

    data_format.extend([
        (READ_EXPORT, "graphic_deltas", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=GraphicDelta,
            length="delta_count",
        )),

        # if attack_sound_used:
        (READ, "graphic_attack_sounds", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=GraphicAttackSound,
            length=lambda o: "angle_count" if o.attack_sound_used != 0 else 0,
        )),
    ])

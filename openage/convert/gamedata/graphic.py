# Copyright 2013-2019 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from openage.convert.dataformat.version_detect import GameEdition

from ..dataformat.genie_structure import GenieStructure
from ..dataformat.member_access import READ, READ_EXPORT, SKIP
from ..dataformat.read_members import SubdataMember, EnumLookupMember
from ..dataformat.value_members import MemberTypes as StorageType


class GraphicDelta(GenieStructure):
    name_struct        = "graphic_delta"
    name_struct_file   = "graphic"
    struct_description = "delta definitions for ingame graphics files."

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_EXPORT, "graphic_id", StorageType.ID_MEMBER, "int16_t"),
            (SKIP, "padding_1", StorageType.INT_MEMBER, "int16_t"),
            (SKIP, "sprite_ptr", StorageType.INT_MEMBER, "int32_t"),
            (READ_EXPORT, "offset_x", StorageType.INT_MEMBER, "int16_t"),
            (READ_EXPORT, "offset_y", StorageType.INT_MEMBER, "int16_t"),
            (READ, "display_angle", StorageType.INT_MEMBER, "int16_t"),
            (SKIP, "padding_2", StorageType.INT_MEMBER, "int16_t"),
        ]

        return data_format


class DE2SoundProp(GenieStructure):
    name_struct        = "de2_sound_prop"
    name_struct_file   = "graphic"
    struct_description = "DE2 sound id and delay definition for graphics sounds."

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            (READ, "sound_delay0", StorageType.INT_MEMBER, "int16_t"),
            (READ, "sound_id0", StorageType.ID_MEMBER, "int16_t"),
            (READ, "wwise_sound0", StorageType.ID_MEMBER, "uint32_t"),
            (READ, "sound_delay1", StorageType.INT_MEMBER, "int16_t"),
            (READ, "wwise_sound1", StorageType.ID_MEMBER, "uint32_t"),
            (READ, "sound_id1", StorageType.ID_MEMBER, "int16_t"),
            (READ, "sound_delay2", StorageType.INT_MEMBER, "int16_t"),
            (READ, "wwise_sound2", StorageType.ID_MEMBER, "uint32_t"),
            (READ, "sound_id2", StorageType.ID_MEMBER, "int16_t"),
        ]

        return data_format


class SoundProp(GenieStructure):
    name_struct        = "sound_prop"
    name_struct_file   = "graphic"
    struct_description = "sound id and delay definition for graphics sounds."

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            (READ, "sound_delay", StorageType.INT_MEMBER, "int16_t"),
            (READ, "sound_id", StorageType.ID_MEMBER, "int16_t"),
        ]

        return data_format


class GraphicAttackSound(GenieStructure):
    name_struct        = "graphic_attack_sound"
    name_struct_file   = "graphic"
    struct_description = "attack sounds for a given graphics file."

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        if game_version[0] is GameEdition.AOE2DE:
            data_format = [
                (READ, "sound_props", StorageType.ARRAY_CONTAINER, SubdataMember(
                    ref_type=DE2SoundProp,
                    length=1,
                )),
            ]

        else:
            data_format = [
                (READ, "sound_props", StorageType.ARRAY_CONTAINER, SubdataMember(
                    ref_type=SoundProp,
                    length=3,
                )),
            ]

        return data_format


class Graphic(GenieStructure):
    name_struct        = "graphic"
    name_struct_file   = name_struct
    struct_description = "metadata for ingame graphics files."

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = []

        # internal name: e.g. ARRG2NNE = archery range feudal Age north european
        if game_version[0] in (GameEdition.AOE1DE, GameEdition.AOE2DE):
            data_format.extend([
                (SKIP, "name_len_debug", StorageType.INT_MEMBER, "uint16_t"),
                (READ_EXPORT, "name_len", StorageType.INT_MEMBER, "uint16_t"),
                (READ_EXPORT, "name", StorageType.STRING_MEMBER, "char[name_len]"),
                (SKIP, "filename_len_debug", StorageType.INT_MEMBER, "uint16_t"),
                (READ_EXPORT, "filename_len", StorageType.INT_MEMBER, "uint16_t"),
                (READ_EXPORT, "filename", StorageType.STRING_MEMBER, "char[filename_len]"),
            ])
            if game_version[0] is GameEdition.AOE2DE:
                data_format.extend([
                    (SKIP, "particle_effect_name_len_debug", StorageType.INT_MEMBER, "uint16_t"),
                    (READ_EXPORT, "particle_effect_name_len", StorageType.INT_MEMBER, "uint16_t"),
                    (READ_EXPORT, "particle_effect_name", StorageType.STRING_MEMBER, "char[particle_effect_name_len]"),
                ])
            if game_version[0] is GameEdition.AOE1DE:
                data_format.extend([
                    (READ_EXPORT, "first_frame", StorageType.ID_MEMBER, "uint16_t"),
                ])

        elif game_version[0] is GameEdition.SWGB:
            data_format.extend([
                (READ_EXPORT, "name", StorageType.STRING_MEMBER, "char[25]"),
                (READ_EXPORT, "filename", StorageType.STRING_MEMBER, "char[25]"),
            ])
        else:
            data_format.extend([
                (READ_EXPORT, "name", StorageType.STRING_MEMBER, "char[21]"),
                (READ_EXPORT, "filename", StorageType.STRING_MEMBER, "char[13]"),
            ])

        data_format.extend([
            (READ_EXPORT, "slp_id", StorageType.ID_MEMBER, "int32_t"),             # id of the graphics file in the drs
            (SKIP, "is_loaded", StorageType.BOOLEAN_MEMBER, "int8_t"),             # unused
            (SKIP, "old_color_flag", StorageType.BOOLEAN_MEMBER, "int8_t"),        # unused
            (READ_EXPORT, "layer", StorageType.ID_MEMBER, EnumLookupMember(       # originally 40 layers, higher -> drawn on top
                raw_type    = "int8_t",  # -> same layer -> order according to map position.
                type_name   = "graphics_layer",
                lookup_dict = {
                    0: "TERRAIN",      # cliff
                    1: "GRASS_PATCH",
                    2: "DE2_CLIFF",
                    3: "AOE1_DIRT",
                    4: "DE1_DESTRUCTION",
                    5: "SHADOW",       # farm fields as well
                    6: "RUBBLE",
                    7: "PLANT",
                    9: "SWGB_EFFECT",
                    10: "UNIT_LOW",    # constructions, dead units, tree stumps, flowers, paths
                    11: "FISH",
                    18: "SWGB_LAYER1",
                    19: "CRATER",      # rugs
                    20: "UNIT",        # buildings, units, damage flames, animations (mill)
                    21: "BLACKSMITH",  # blacksmith smoke
                    22: "BIRD",        # hawk
                    30: "PROJECTILE",  # and explosions
                    31: "SWGB_FLYING",
                }
            )),
            (READ_EXPORT, "player_color_force_id", StorageType.ID_MEMBER, "int8_t"),    # force given player color
            (READ_EXPORT, "adapt_color", StorageType.INT_MEMBER, "int8_t"),             # playercolor can be changed on sight (like sheep)
            (READ_EXPORT, "transparent_selection", StorageType.INT_MEMBER, "uint8_t"),  # loop animation
            (READ, "coordinates", StorageType.ARRAY_INT, "int16_t[4]"),
            (READ_EXPORT, "delta_count", StorageType.INT_MEMBER, "uint16_t"),
            (READ_EXPORT, "sound_id", StorageType.ID_MEMBER, "int16_t"),
        ])

        if game_version[0] is GameEdition.AOE2DE:
            data_format.extend([
                (READ_EXPORT, "wwise_sound_id", StorageType.ID_MEMBER, "uint32_t"),
            ])

        data_format.extend([
            (READ_EXPORT, "attack_sound_used", StorageType.INT_MEMBER, "uint8_t"),
            (READ_EXPORT, "frame_count", StorageType.INT_MEMBER, "uint16_t"),           # number of frames per angle
            (READ_EXPORT, "angle_count", StorageType.INT_MEMBER, "uint16_t"),           # number of heading angles stored, some of the frames must be mirrored
            (READ, "speed_adjust", StorageType.FLOAT_MEMBER, "float"),                  # multiplies the speed of the unit this graphic is applied to
            (READ_EXPORT, "frame_rate", StorageType.FLOAT_MEMBER, "float"),             # how long a frame is displayed
            (READ_EXPORT, "replay_delay", StorageType.FLOAT_MEMBER, "float"),           # seconds to wait before current_frame=0 again
            (READ_EXPORT, "sequence_type", StorageType.ID_MEMBER, "int8_t"),
            (READ_EXPORT, "graphic_id", StorageType.ID_MEMBER, "int16_t"),
            (READ_EXPORT, "mirroring_mode", StorageType.ID_MEMBER, "int8_t"),
        ])

        if game_version[0] not in (GameEdition.ROR, GameEdition.AOE1DE):
            # sprite editor thing for AoK
            data_format.append((SKIP, "editor_flag", StorageType.BOOLEAN_MEMBER, "int8_t"))

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

        return data_format

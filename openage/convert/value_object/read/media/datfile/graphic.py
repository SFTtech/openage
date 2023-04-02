# Copyright 2013-2023 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R
from __future__ import annotations
import typing

from functools import cache

from ...genie_structure import GenieStructure
from ....read.member_access import READ, READ_GEN, SKIP
from ....read.read_members import SubdataMember, EnumLookupMember
from ....read.value_members import StorageType
from .lookup_dicts import GRAPHICS_LAYER

if typing.TYPE_CHECKING:
    from openage.convert.value_object.init.game_version import GameVersion
    from openage.convert.value_object.read.member_access import MemberAccess
    from openage.convert.value_object.read.read_members import ReadMember


class GraphicDelta(GenieStructure):

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, "graphic_id", StorageType.ID_MEMBER, "int16_t"),
            (SKIP, "padding_1", StorageType.INT_MEMBER, "int16_t"),
            (SKIP, "sprite_ptr", StorageType.INT_MEMBER, "int32_t"),
            (READ_GEN, "offset_x", StorageType.INT_MEMBER, "int16_t"),
            (READ_GEN, "offset_y", StorageType.INT_MEMBER, "int16_t"),
            (READ_GEN, "display_angle", StorageType.INT_MEMBER, "int16_t"),
            (SKIP, "padding_2", StorageType.INT_MEMBER, "int16_t"),
        ]

        return data_format


class DE2SoundProp(GenieStructure):

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (SKIP, "sound_delay0", StorageType.INT_MEMBER, "int16_t"),
            (SKIP, "sound_id0", StorageType.ID_MEMBER, "int16_t"),
            (SKIP, "wwise_sound0", StorageType.ID_MEMBER, "uint32_t"),
            (SKIP, "sound_delay1", StorageType.INT_MEMBER, "int16_t"),
            (SKIP, "wwise_sound1", StorageType.ID_MEMBER, "uint32_t"),
            (SKIP, "sound_id1", StorageType.ID_MEMBER, "int16_t"),
            (SKIP, "sound_delay2", StorageType.INT_MEMBER, "int16_t"),
            (SKIP, "wwise_sound2", StorageType.ID_MEMBER, "uint32_t"),
            (SKIP, "sound_id2", StorageType.ID_MEMBER, "int16_t"),
        ]

        return data_format


class SoundProp(GenieStructure):

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (SKIP, "sound_delay", StorageType.INT_MEMBER, "int16_t"),
            (SKIP, "sound_id", StorageType.ID_MEMBER, "int16_t"),
        ]

        return data_format


class GraphicAttackSound(GenieStructure):

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        if game_version.edition.game_id == "AOE2DE":
            data_format = [
                (SKIP, "sound_props", StorageType.ARRAY_CONTAINER, SubdataMember(
                    ref_type=DE2SoundProp,
                    length=1,
                )),
            ]

        else:
            data_format = [
                (SKIP, "sound_props", StorageType.ARRAY_CONTAINER, SubdataMember(
                    ref_type=SoundProp,
                    length=3,
                )),
            ]

        return data_format


class Graphic(GenieStructure):

    dynamic_load = True

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = []

        # internal name: e.g. ARRG2NNE = archery range feudal Age north european
        if game_version.edition.game_id in ("AOE1DE", "AOE2DE"):
            data_format.extend([
                (SKIP, "name_len_debug", StorageType.INT_MEMBER, "uint16_t"),
                (READ, "name_len", StorageType.INT_MEMBER, "uint16_t"),
                (SKIP, "name", StorageType.STRING_MEMBER, "char[name_len]"),
                (SKIP, "filename_len_debug", StorageType.INT_MEMBER, "uint16_t"),
                (READ, "filename_len", StorageType.INT_MEMBER, "uint16_t"),
                (READ_GEN, "filename", StorageType.STRING_MEMBER, "char[filename_len]"),
            ])
            if game_version.edition.game_id == "AOE2DE":
                data_format.extend([
                    (SKIP, "particle_effect_name_len_debug", StorageType.INT_MEMBER, "uint16_t"),
                    (READ, "particle_effect_name_len", StorageType.INT_MEMBER, "uint16_t"),
                    (READ_GEN, "particle_effect_name", StorageType.STRING_MEMBER,
                     "char[particle_effect_name_len]"),
                ])
            if game_version.edition.game_id == "AOE1DE":
                data_format.extend([
                    (READ_GEN, "first_frame", StorageType.ID_MEMBER, "uint16_t"),
                ])

        elif game_version.edition.game_id == "SWGB":
            data_format.extend([
                (SKIP, "name", StorageType.STRING_MEMBER, "char[25]"),
                (READ_GEN, "filename", StorageType.STRING_MEMBER, "char[25]"),
            ])
        else:
            data_format.extend([
                (SKIP, "name", StorageType.STRING_MEMBER, "char[21]"),
                (READ_GEN, "filename", StorageType.STRING_MEMBER, "char[13]"),
            ])

        data_format.extend([
            # id of the graphics file in the drs
            (READ_GEN, "slp_id", StorageType.ID_MEMBER, "int32_t"),
            (SKIP, "is_loaded", StorageType.BOOLEAN_MEMBER, "int8_t"),             # unused
            (SKIP, "old_color_flag", StorageType.BOOLEAN_MEMBER, "int8_t"),        # unused
            (READ_GEN, "layer", StorageType.ID_MEMBER, EnumLookupMember(       # originally 40 layers, higher -> drawn on top
                raw_type    = "int8_t",  # -> same layer -> order according to map position.
                type_name   = "graphics_layer",
                lookup_dict = GRAPHICS_LAYER
            )),
            (SKIP, "player_color_force_id", StorageType.ID_MEMBER,
             "int8_t"),    # force given player color
            # playercolor can be changed on sight (like sheep)
            (SKIP, "adapt_color", StorageType.INT_MEMBER, "int8_t"),
            (SKIP, "transparent_selection", StorageType.INT_MEMBER, "uint8_t"),  # loop animation
            (READ, "coordinates", StorageType.ARRAY_INT, "int16_t[4]"),
            (READ, "delta_count", StorageType.INT_MEMBER, "uint16_t"),
            (SKIP, "sound_id", StorageType.ID_MEMBER, "int16_t"),
        ])

        if game_version.edition.game_id == "AOE2DE":
            data_format.extend([
                (READ_GEN, "wwise_sound_id", StorageType.ID_MEMBER, "uint32_t"),
            ])

        data_format.extend([
            (READ, "attack_sound_used", StorageType.INT_MEMBER, "uint8_t"),
            (READ_GEN, "frame_count", StorageType.INT_MEMBER,
             "uint16_t"),           # number of frames per angle
            # number of heading angles stored, some of the frames must be mirrored
            (READ_GEN, "angle_count", StorageType.INT_MEMBER, "uint16_t"),
            # multiplies the speed of the unit this graphic is applied to
            (SKIP, "speed_adjust", StorageType.FLOAT_MEMBER, "float"),
            (READ_GEN, "frame_rate", StorageType.FLOAT_MEMBER,
             "float"),             # how long a frame is displayed
            # seconds to wait before current_frame=0 again
            (READ_GEN, "replay_delay", StorageType.FLOAT_MEMBER, "float"),
            (READ_GEN, "sequence_type", StorageType.ID_MEMBER, "int8_t"),
            (READ_GEN, "graphic_id", StorageType.ID_MEMBER, "int16_t"),
            (READ_GEN, "mirroring_mode", StorageType.ID_MEMBER, "int8_t"),
        ])

        if game_version.edition.game_id not in ("ROR", "AOE1DE"):
            # sprite editor thing for AoK
            data_format.append((SKIP, "editor_flag", StorageType.BOOLEAN_MEMBER, "int8_t"))

        data_format.extend([
            (READ_GEN, "graphic_deltas", StorageType.ARRAY_CONTAINER, SubdataMember(
                ref_type=GraphicDelta,
                length="delta_count",
            )),

            # if attack_sound_used:
            (SKIP, "graphic_attack_sounds", StorageType.ARRAY_CONTAINER, SubdataMember(
                ref_type=GraphicAttackSound,
                length=lambda o: "angle_count" if o.attack_sound_used != 0 else 0,
            )),
        ])

        return data_format

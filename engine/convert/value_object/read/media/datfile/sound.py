# Copyright 2013-2023 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R
from __future__ import annotations
import typing

from functools import cache

from ...genie_structure import GenieStructure
from ....read.member_access import READ_GEN, READ, SKIP
from ....read.read_members import SubdataMember
from ....read.value_members import StorageType

if typing.TYPE_CHECKING:
    from engine.convert.value_object.init.game_version import GameVersion
    from engine.convert.value_object.read.member_access import MemberAccess
    from engine.convert.value_object.read.read_members import ReadMember


class SoundItem(GenieStructure):

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

        if game_version.edition.game_id in ("AOE1DE", "AOE2DE"):
            data_format.extend([
                (SKIP, "name_len_debug", StorageType.INT_MEMBER, "uint16_t"),
                (READ, "name_len", StorageType.INT_MEMBER, "uint16_t"),
                (SKIP, "name", StorageType.STRING_MEMBER, "char[name_len]"),
            ])
        elif game_version.edition.game_id == "SWGB":
            data_format.extend([
                (READ_GEN, "filename", StorageType.STRING_MEMBER, "char[27]"),
            ])
        else:
            data_format.extend([
                (READ_GEN, "filename", StorageType.STRING_MEMBER, "char[13]"),
            ])

        data_format.extend([
            (READ_GEN, "resource_id", StorageType.ID_MEMBER, "int32_t"),
            (READ_GEN, "probablilty", StorageType.INT_MEMBER, "int16_t"),
        ])

        if game_version.edition.game_id not in ("ROR", "AOE1DE"):
            data_format.extend([
                (READ_GEN, "civilization_id", StorageType.ID_MEMBER, "int16_t"),
                (READ_GEN, "icon_set", StorageType.ID_MEMBER, "int16_t"),
            ])

        return data_format


class Sound(GenieStructure):

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
        data_format = [
            (READ_GEN, "sound_id", StorageType.ID_MEMBER, "int16_t"),
            (READ_GEN, "play_delay", StorageType.INT_MEMBER, "int16_t"),
            (READ, "file_count", StorageType.INT_MEMBER, "uint16_t"),
            (SKIP, "cache_time", StorageType.INT_MEMBER, "int32_t"),                   # always 300000
        ]

        if game_version.edition.game_id in ("AOE1DE", "AOE2DE"):
            data_format.extend([
                (READ_GEN, "total_probability", StorageType.ID_MEMBER, "int16_t"),
            ])

        data_format.extend([
            (READ_GEN, "sound_items", StorageType.ARRAY_CONTAINER, SubdataMember(
                ref_type=SoundItem,
                ref_to="id",
                length="file_count",
            )),
        ])

        return data_format

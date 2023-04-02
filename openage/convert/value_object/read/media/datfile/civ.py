# Copyright 2013-2023 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R
from __future__ import annotations
import typing

from functools import cache

from . import unit
from ...genie_structure import GenieStructure
from ....read.member_access import READ, READ_GEN, SKIP
from ....read.read_members import MultisubtypeMember, EnumLookupMember
from ....read.value_members import StorageType

if typing.TYPE_CHECKING:
    from openage.convert.value_object.init.game_version import GameVersion
    from openage.convert.value_object.read.member_access import MemberAccess
    from openage.convert.value_object.read.read_members import ReadMember


class Civ(GenieStructure):

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
            # always 1
            (SKIP, "player_type", StorageType.INT_MEMBER, "int8_t"),
        ]

        if game_version.edition.game_id in ("AOE1DE", "AOE2DE"):
            data_format.extend([
                (SKIP, "name_len_debug", StorageType.INT_MEMBER, "uint16_t"),
                (READ, "name_len", StorageType.INT_MEMBER, "uint16_t"),
                (SKIP, "name", StorageType.STRING_MEMBER, "char[name_len]"),
            ])
        else:
            data_format.extend([
                (SKIP, "name", StorageType.STRING_MEMBER, "char[20]"),
            ])

        data_format.extend([
            (READ, "resources_count", StorageType.INT_MEMBER, "uint16_t"),
            # links to effect bundle id (to apply its effects)
            (READ_GEN, "tech_tree_id", StorageType.ID_MEMBER, "int16_t"),
        ])

        if game_version.edition.game_id not in ("ROR", "AOE1DE"):
            # links to tech id as well
            data_format.append((READ_GEN, "team_bonus_id", StorageType.ID_MEMBER, "int16_t"))

            if game_version.edition.game_id == "SWGB":
                data_format.extend([
                    (READ_GEN, "name2", StorageType.STRING_MEMBER, "char[20]"),
                    (READ_GEN, "unique_unit_techs", StorageType.ARRAY_ID, "int16_t[4]"),
                ])

        data_format.extend([
            (READ_GEN, "resources", StorageType.ARRAY_FLOAT, "float[resources_count]"),
            # building icon set, trade cart graphics, changes no other graphics
            (READ_GEN, "icon_set", StorageType.ID_MEMBER, "int8_t"),
            (READ, "unit_count", StorageType.INT_MEMBER, "uint16_t"),
            (READ, "unit_offsets", StorageType.ARRAY_ID, "int32_t[unit_count]"),

            (READ_GEN, "units", StorageType.ARRAY_CONTAINER, MultisubtypeMember(
                type_name          = "unit_types",
                subtype_definition = (READ_GEN, "unit_type", StorageType.ID_MEMBER, EnumLookupMember(
                    type_name      = "unit_type_id",
                    lookup_dict    = unit.unit_type_lookup,
                    raw_type       = "int8_t",
                )),
                class_lookup       = unit.unit_type_class_lookup,
                length             = "unit_count",
                offset_to          = ("unit_offsets", lambda o: o > 0),
            )),
        ])

        return data_format

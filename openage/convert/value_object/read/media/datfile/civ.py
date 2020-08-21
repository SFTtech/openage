# Copyright 2013-2020 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R
from . import unit
from .....entity_object.conversion.genie_structure import GenieStructure
from ....init.game_version import GameEdition
from ....read.member_access import READ, READ_GEN, SKIP
from ....read.read_members import MultisubtypeMember, EnumLookupMember
from ....read.value_members import MemberTypes as StorageType


class Civ(GenieStructure):
    name_struct = "civilisation"
    name_struct_file = name_struct
    struct_description = "describes a civilisation."

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            # always 1
            (SKIP, "player_type", StorageType.INT_MEMBER, "int8_t"),
        ]

        if game_version[0] in (GameEdition.AOE1DE, GameEdition.AOE2DE):
            data_format.extend([
                (SKIP, "name_len_debug", StorageType.INT_MEMBER, "uint16_t"),
                (READ, "name_len", StorageType.INT_MEMBER, "uint16_t"),
                (READ_GEN, "name", StorageType.STRING_MEMBER, "char[name_len]"),
            ])
        else:
            data_format.extend([
                (READ_GEN, "name", StorageType.STRING_MEMBER, "char[20]"),
            ])

        data_format.extend([
            (READ, "resources_count", StorageType.INT_MEMBER, "uint16_t"),
            # links to effect bundle id (to apply its effects)
            (READ_GEN, "tech_tree_id", StorageType.ID_MEMBER, "int16_t"),
        ])

        if game_version[0] not in (GameEdition.ROR, GameEdition.AOE1DE):
            # links to tech id as well
            data_format.append((READ_GEN, "team_bonus_id", StorageType.ID_MEMBER, "int16_t"))

            if game_version[0] is GameEdition.SWGB:
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

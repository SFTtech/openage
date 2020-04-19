# Copyright 2013-2019 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from . import unit
from ..dataformat.genie_structure import GenieStructure
from ..dataformat.read_members import MultisubtypeMember, EnumLookupMember
from ..dataformat.member_access import READ, READ_EXPORT
from ..dataformat.value_members import MemberTypes as StorageType
from openage.convert.dataformat.version_detect import GameEdition


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
            (READ, "player_type", StorageType.INT_MEMBER, "int8_t"),
            (READ_EXPORT, "name", StorageType.STRING_MEMBER, "char[20]"),
            (READ, "resources_count", StorageType.INT_MEMBER, "uint16_t"),
            # links to effect bundle id (to apply its effects)
            (READ_EXPORT, "tech_tree_id", StorageType.ID_MEMBER, "int16_t"),
        ]

        if game_version[0] is not GameEdition.ROR:
            # links to tech id as well
            data_format.append((READ_EXPORT, "team_bonus_id", StorageType.ID_MEMBER, "int16_t"))

        if game_version[0] is GameEdition.SWGB:
            data_format.extend([
                (READ, "name2", StorageType.STRING_MEMBER, "char[20]"),
                (READ, "unique_unit_techs", StorageType.ARRAY_ID, "int16_t[4]"),
            ])

        data_format.extend([
            (READ, "resources", StorageType.ARRAY_FLOAT, "float[resources_count]"),
            (READ, "icon_set", StorageType.ID_MEMBER, "int8_t"),                      # building icon set, trade cart graphics, changes no other graphics
            (READ_EXPORT, "unit_count", StorageType.INT_MEMBER, "uint16_t"),
            (READ, "unit_offsets", StorageType.ARRAY_ID, "int32_t[unit_count]"),

            (READ_EXPORT, "units", StorageType.ARRAY_CONTAINER, MultisubtypeMember(
                type_name          = "unit_types",
                subtype_definition = (READ, "unit_type", StorageType.ID_MEMBER, EnumLookupMember(
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

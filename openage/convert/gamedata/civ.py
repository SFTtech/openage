# Copyright 2013-2017 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from . import unit
from ..dataformat.exportable import Exportable
from openage.convert.dataformat.read_members import MultisubtypeMember, EnumLookupMember
from ..dataformat.member_access import READ, READ_EXPORT


class Civ(Exportable):
    name_struct = "civilisation"
    name_struct_file = name_struct
    struct_description = "describes a civilisation."

    data_format = [
        (READ, "player_type", "int8_t"),                   # always 1
        (READ_EXPORT, "name", "char[20]"),
        (READ, "resources_count", "uint16_t"),
        (READ_EXPORT, "tech_tree_id",  "int16_t"),         # links to tech id (to apply its effects)
    ]

    # TODO: Enable conversion for AOE1; replace "team_bonus_id"
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.append((READ_EXPORT, "team_bonus_id", "int16_t"))
    # ===========================================================================
    data_format.append((READ_EXPORT, "team_bonus_id", "int16_t"))         # links to tech id as well

    # TODO: Enable conversion for SWGB
    # ===========================================================================
    # if (GameVersion.swgb_10 or GameVersion.swgb_cc) in game_versions:
    #     data_format.extend([
    #         (READ, "name2", "char[20]"),
    #         (READ, "unique_unit_techs", "int16_t[4]"),
    #     ])
    # ===========================================================================

    data_format.extend([
        (READ, "resources", "float[resources_count]"),
        (READ, "icon_set", "int8_t"),                      # building icon set, trade cart graphics, changes no other graphics
        (READ_EXPORT, "unit_count", "uint16_t"),
        (READ, "unit_offsets", "int32_t[unit_count]"),

        (READ_EXPORT, "units", MultisubtypeMember(
            type_name          = "unit_types",
            subtype_definition = (READ, "unit_type", EnumLookupMember(
                type_name      = "unit_type_id",
                lookup_dict    = unit.unit_type_lookup,
                raw_type       = "int8_t",
            )),
            class_lookup       = unit.unit_type_class_lookup,
            length             = "unit_count",
            offset_to          = ("unit_offsets", lambda o: o > 0),
        )),
    ])

# Copyright 2013-2015 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from . import unit
from ..dataformat.exportable import Exportable
from ..dataformat.members import MultisubtypeMember, EnumLookupMember
from ..dataformat.member_access import READ, READ_EXPORT


class Civ(Exportable):
    name_struct = "civilisation"
    name_struct_file = name_struct
    struct_description = "describes a civilisation."

    data_format = (
        (READ, "enabled", "int8_t"),
        (READ_EXPORT, "name", "char[20]"),
        (READ, "resources_count", "uint16_t"),
        (READ_EXPORT, "tech_tree_id",  "int16_t"),             # links to tech id (to apply its effects)
        (READ_EXPORT, "team_bonus_id", "int16_t"),             # links to tech id as well
        (READ, "resources", "float[resources_count]"),
        (READ, "icon_set", "int8_t"),                          # building icon set, trade cart graphics, changes no other graphics
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
    )

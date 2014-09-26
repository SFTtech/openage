from struct import Struct, unpack_from
from .. import dataformat
from ..util import dbg, zstr

from .empiresdat import endianness
from . import unit


class Civ(dataformat.Exportable):
    name_struct        = "civilisation"
    name_struct_file   = name_struct
    struct_description = "describes one a civilisation."

    data_format = (
        (dataformat.READ, "enabled", "int8_t"),
        (dataformat.READ_EXPORT, "name", "char[20]"),
        (dataformat.READ, "ressources_count", "uint16_t"),
        (dataformat.READ_EXPORT, "tech_tree_id",  "int16_t"),             #links to tech id (to apply its effects)
        (dataformat.READ_EXPORT, "team_bonus_id", "int16_t"),             #links to tech id as well
        (dataformat.READ, "ressources", "float[ressources_count]"),
        (dataformat.READ, "graphic_set", "int8_t"),                       #building icon set, trade cart graphics, changes no other graphics
        (dataformat.READ_EXPORT, "unit_count", "uint16_t"),
        (dataformat.READ, "unit_offsets", "int32_t[unit_count]"),

        (dataformat.READ_EXPORT, "units", dataformat.MultisubtypeMember(
            type_name          = "unit_types",
            subtype_definition = (dataformat.READ, "unit_type", dataformat.EnumLookupMember(
                type_name      = "unit_type_id",
                lookup_dict    = unit.unit_type_lookup,
                raw_type       = "int8_t",
            )),
            class_lookup       = unit.unit_type_class_lookup,
            length             = "unit_count",
            offset_to          = ("unit_offsets", lambda o: o > 0),
        )),
    )

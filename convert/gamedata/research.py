import dataformat
from struct import Struct, unpack_from
from util import dbg, zstr

from .empiresdat import endianness


class ResearchRessourceCost(dataformat.Exportable):
    name_struct        = "research_ressource_cost"
    name_struct_file   = "research"
    struct_description = "amount definition for a single type ressource for researches."

    data_format = (
        (dataformat.READ, "a", "int16_t"),
        (dataformat.READ, "b", "int16_t"),
        (dataformat.READ, "c", "int8_t"),
    )


class Research(dataformat.Exportable):
    name_struct        = "research"
    name_struct_file   = "research"
    struct_description = "one researchable technology."

    data_format = (
        (dataformat.READ, "required_techs", "int16_t[6]"),
        (dataformat.READ, "research_ressource_costs", dataformat.SubdataMember(
            ref_type=ResearchRessourceCost,
            length=3,
        )),
        (dataformat.READ, "required_tech_count", "int16_t"),
        (dataformat.READ, "civilisation_id", "int16_t"),
        (dataformat.READ, "full_tech_mode", "int16_t"),
        (dataformat.READ, "research_location_id", "int16_t"),
        (dataformat.READ, "language_dll_name", "uint16_t"),
        (dataformat.READ, "languate_dll_description", "uint16_t"),
        (dataformat.READ, "research_time", "int16_t"),
        (dataformat.READ, "tech_id", "int16_t"),
        (dataformat.READ, "tech_type", "int16_t"),
        (dataformat.READ, "icon_id", "int16_t"),
        (dataformat.READ, "button_id", "int8_t"),
        (dataformat.READ, "pointers", "int32_t[3]"),
        (dataformat.READ, "name_length", "uint16_t"),
        (dataformat.READ, "name", "char[name_length]"),
    )

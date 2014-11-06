# Copyright 2013-2014 the openage authors. See copying.md for legal info.

from .. import dataformat
from struct import Struct, unpack_from
from ..util import dbg, zstr

from .empiresdat import endianness


class ResearchResourceCost(dataformat.Exportable):
    name_struct        = "research_resource_cost"
    name_struct_file   = "research"
    struct_description = "amount definition for a single type resource for researches."

    data_format = (
        (dataformat.READ, "resource_id", "int16_t"),  #see unit/resource_cost, TODO: type xref
        (dataformat.READ, "amount", "int16_t"),
        (dataformat.READ, "enabled", "int8_t"),
    )


class Research(dataformat.Exportable):
    name_struct        = "research"
    name_struct_file   = "research"
    struct_description = "one researchable technology."

    data_format = (
        (dataformat.READ, "required_techs", "int16_t[6]"),         #research ids of techs that are required for activating the possible research
        (dataformat.READ, "research_resource_costs", dataformat.SubdataMember(
            ref_type=ResearchResourceCost,
            length=3,
        )),
        (dataformat.READ, "required_tech_count", "int16_t"),       #a subset of the above required techs may be sufficient
        (dataformat.READ, "civilisation_id", "int16_t"),
        (dataformat.READ, "full_tech_mode", "int16_t"),            #1: research is available when the full tech tree is activated on game start, 0: not
        (dataformat.READ, "research_location_id", "int16_t"),      #unit id, where the tech will appear to be researched
        (dataformat.READ, "language_dll_name", "uint16_t"),
        (dataformat.READ, "languate_dll_description", "uint16_t"),
        (dataformat.READ, "research_time", "int16_t"),             #time in seconds that are needed to finish this research
        (dataformat.READ, "tech_effect_id", "int16_t"),            #techage id that actually contains the research effect information
        (dataformat.READ, "tech_type", "int16_t"),                 #0: research is not dependant on current age, 2: implied by new age
        (dataformat.READ, "icon_id", "int16_t"),                   #frame id - 1 in icon slp (57029)
        (dataformat.READ, "button_id", "int8_t"),
        (dataformat.READ, "pointers", "int32_t[3]"),               #[0]: 100000+lang_dll_name [1]: 149000+lang_dll_description [2]: -1
        (dataformat.READ, "name_length", "uint16_t"),
        (dataformat.READ, "name", "char[name_length]"),
    )

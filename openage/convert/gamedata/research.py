# Copyright 2013-2015 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from ..dataformat.exportable import Exportable
from ..dataformat.members import SubdataMember
from ..dataformat.member_access import READ, READ_UNKNOWN


class ResearchResourceCost(Exportable):
    name_struct        = "research_resource_cost"
    name_struct_file   = "research"
    struct_description = "amount definition for a single type resource for researches."

    data_format = [
        (READ, "resource_id", "int16_t"),  # see unit/resource_cost, TODO: type xref
        (READ, "amount", "int16_t"),
        (READ, "enabled", "int8_t"),
    ]


class Research(Exportable):
    name_struct        = "research"
    name_struct_file   = "research"
    struct_description = "one researchable technology."

    data_format = [
        (READ, "required_techs", "int16_t[6]"),         # research ids of techs that are required for activating the possible research
        (READ, "research_resource_costs", SubdataMember(
            ref_type=ResearchResourceCost,
            length=3,
        )),
        (READ, "required_tech_count", "int16_t"),       # a subset of the above required techs may be sufficient, this defines the minimum amount
        (READ, "civilisation_id", "int16_t"),           # id of the civ that gets this technology
        (READ, "full_tech_mode", "int16_t"),            # 1: research is available when the full tech tree is activated on game start, 0: not
        (READ, "research_location_id", "int16_t"),      # unit id, where the tech will appear to be researched
        (READ, "language_dll_name", "uint16_t"),
        (READ, "language_dll_description", "uint16_t"),
        (READ, "research_time", "int16_t"),             # time in seconds that are needed to finish this research
        (READ, "tech_effect_id", "int16_t"),            # techage id that actually contains the research effect information
        (READ, "tech_type", "int16_t"),                 # 0: research is not dependant on current age, 2: implied by new age
        (READ, "icon_id", "int16_t"),                   # frame id - 1 in icon slp (57029)
        (READ, "button_id", "int8_t"),                  # button id as defined in the unit.py button matrix
        (READ, "language_dll_help", "int32_t"),         # 100000 + the language file id for the name/description
        (READ, "language_dll_techtree", "int32_t"),     # 149000 + lang_dll_description
        (READ_UNKNOWN, None, "int32_t"),                # -1
        (READ, "name_length", "uint16_t"),
        (READ, "name", "char[name_length]"),
    ]

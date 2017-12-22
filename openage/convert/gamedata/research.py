# Copyright 2013-2017 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from ..game_versions import GameVersion
from ..dataformat.exportable import Exportable
from ..dataformat.members import SubdataMember
from ..dataformat.member_access import READ


class ResearchResourceCost(Exportable):
    name_struct        = "research_resource_cost"
    name_struct_file   = "research"
    struct_description = "amount definition for a single type resource for researches."

    data_format = []
    data_format.append((READ, "resource_id", "int16_t"))  # see unit/resource_cost, TODO: type xref
    data_format.append((READ, "amount", "int16_t"))
    data_format.append((READ, "enabled", "int8_t"))


class Research(Exportable):
    name_struct        = "research"
    name_struct_file   = "research"
    struct_description = "one researchable technology."

    data_format = []
    data_format.append((READ, "required_techs", "int16_t[6]"))         # research ids of techs that are required for activating the possible research
    data_format.append((READ, "research_resource_costs", SubdataMember(
            ref_type=ResearchResourceCost,
            length=3,
        )))
    data_format.append((READ, "required_tech_count", "int16_t"))       # a subset of the above required techs may be sufficient, this defines the minimum amount
    data_format.append((READ, "civilisation_id", "int16_t"))           # id of the civ that gets this technology
    data_format.append((READ, "full_tech_mode", "int16_t"))            # 1: research is available when the full tech tree is activated on game start, 0: not
    data_format.append((READ, "research_location_id", "int16_t"))      # unit id, where the tech will appear to be researched
    data_format.append((READ, "language_dll_name", "uint16_t"))
    data_format.append((READ, "language_dll_description", "uint16_t"))
    data_format.append((READ, "research_time", "int16_t"))             # time in seconds that are needed to finish this research
    data_format.append((READ, "tech_effect_id", "int16_t"))            # techage id that actually contains the research effect information
    data_format.append((READ, "tech_type", "int16_t"))                 # 0: research is not dependant on current age, 2: implied by new age
    data_format.append((READ, "icon_id", "int16_t"))                   # frame id - 1 in icon slp (57029)
    data_format.append((READ, "button_id", "int8_t"))                  # button id as defined in the unit.py button matrix
    data_format.append((READ, "language_dll_help", "int32_t"))         # 100000 + the language file id for the name/description
    data_format.append((READ, "language_dll_techtree", "int32_t"))     # 149000 + lang_dll_description
    data_format.append((READ, "hotkey", "int32_t"))                    # -1 for every tech
    data_format.append((READ, "name_length", "uint16_t"))
    data_format.append((READ, "name", "char[name_length]"))

    # TODO: Enable conversion for SWGB
    #===========================================================================
    # if (GameVersion.swgb_10 or GameVersion.swgb_cc) in game_versions:
    #     data_format.append((READ, "name2_length", "uint16_t"))
    #     data_format.append((READ, "name2", "char[name2_length]"))
    #===========================================================================

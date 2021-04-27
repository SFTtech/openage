# Copyright 2013-2021 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from .....entity_object.conversion.genie_structure import GenieStructure
from ....read.member_access import READ, READ_GEN, SKIP
from ....read.read_members import SubdataMember, EnumLookupMember
from ....read.value_members import MemberTypes as StorageType
from .lookup_dicts import RESOURCE_TYPES


class TechResourceCost(GenieStructure):

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, "type_id", StorageType.ID_MEMBER, EnumLookupMember(
                raw_type="int16_t",
                type_name="resource_types",
                lookup_dict=RESOURCE_TYPES
            )),  # see unit/resource_cost
            (READ_GEN, "amount", StorageType.INT_MEMBER, "int16_t"),
            (READ_GEN, "enabled", StorageType.BOOLEAN_MEMBER, "int8_t"),
        ]

        return data_format


class Tech(GenieStructure):

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        if game_version[0].game_id not in ("ROR", "AOE1DE"):
            data_format = [
                # research ids of techs that are required for activating the possible research
                (READ_GEN, "required_techs", StorageType.ARRAY_ID, "int16_t[6]"),
            ]
        else:
            data_format = [
                # research ids of techs that are required for activating the possible research
                (READ_GEN, "required_techs", StorageType.ARRAY_ID, "int16_t[4]"),
            ]

        data_format.extend([
            (READ_GEN, "research_resource_costs", StorageType.ARRAY_CONTAINER, SubdataMember(
                ref_type=TechResourceCost,
                length=3,
            )),
            (READ_GEN, "required_tech_count", StorageType.INT_MEMBER, "int16_t"),       # a subset of the above required techs may be sufficient, this defines the minimum amount
        ])

        if game_version[0].game_id not in ("ROR", "AOE1DE"):
            data_format.extend([
                (READ_GEN, "civilization_id", StorageType.ID_MEMBER, "int16_t"),           # id of the civ that gets this technology
                (READ_GEN, "full_tech_mode", StorageType.BOOLEAN_MEMBER, "int16_t"),       # 1: research is available when the full tech tree is activated on game start, 0: not
            ])

        data_format.extend([
            (READ_GEN, "research_location_id", StorageType.ID_MEMBER, "int16_t"),      # unit id, where the tech will appear to be researched
            (READ_GEN, "language_dll_name", StorageType.ID_MEMBER, "uint16_t"),
            (READ_GEN, "language_dll_description", StorageType.ID_MEMBER, "uint16_t"),
            (READ_GEN, "research_time", StorageType.INT_MEMBER, "int16_t"),            # time in seconds that are needed to finish this research
            (READ_GEN, "tech_effect_id", StorageType.ID_MEMBER, "int16_t"),            # techage id that actually contains the research effect information
            (READ_GEN, "tech_type", StorageType.ID_MEMBER, "int16_t"),                 # 0: normal tech, 2: show in Age progress bar
            (READ_GEN, "icon_id", StorageType.ID_MEMBER, "int16_t"),                   # frame id - 1 in icon slp (57029)
            (READ_GEN, "button_id", StorageType.ID_MEMBER, "int8_t"),                  # button id as defined in the unit.py button matrix
            (READ_GEN, "language_dll_help", StorageType.ID_MEMBER, "int32_t"),         # 100000 + the language file id for the name/description
            (READ_GEN, "language_dll_techtree", StorageType.ID_MEMBER, "int32_t"),     # 149000 + lang_dll_description
            (READ_GEN, "hotkey", StorageType.ID_MEMBER, "int32_t"),                    # -1 for every tech
        ])

        if game_version[0].game_id in ("AOE1DE", "AOE2DE"):
            data_format.extend([
                (SKIP, "name_length_debug", StorageType.INT_MEMBER, "uint16_t"),
                (READ, "name_length", StorageType.INT_MEMBER, "uint16_t"),
                (READ_GEN, "name", StorageType.STRING_MEMBER, "char[name_length]"),
            ])

            if game_version[0].game_id == "AOE2DE":
                data_format.extend([
                    (READ_GEN, "repeatable", StorageType.INT_MEMBER, "int8_t"),
                ])

        else:
            data_format.extend([
                (READ, "name_length", StorageType.INT_MEMBER, "uint16_t"),
                (READ_GEN, "name", StorageType.STRING_MEMBER, "char[name_length]"),
            ])

            if game_version[0].game_id == "SWGB":
                data_format.extend([
                    (READ, "name2_length", StorageType.INT_MEMBER, "uint16_t"),
                    (READ_GEN, "name2", StorageType.STRING_MEMBER, "char[name2_length]"),
                ])

        return data_format

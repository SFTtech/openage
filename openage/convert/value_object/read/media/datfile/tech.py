# Copyright 2013-2021 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from .....entity_object.conversion.genie_structure import GenieStructure
from ....read.member_access import READ, READ_GEN, SKIP
from ....read.read_members import SubdataMember, EnumLookupMember
from ....read.value_members import MemberTypes as StorageType
from .lookup_dicts import EFFECT_APPLY_TYPE, CONNECTION_MODE


class Effect(GenieStructure):

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, "type_id", StorageType.ID_MEMBER, EnumLookupMember(
                raw_type="int8_t",
                type_name="effect_apply_type",
                lookup_dict=EFFECT_APPLY_TYPE
            )),
            (READ_GEN, "attr_a", StorageType.INT_MEMBER, "int16_t"),
            (READ_GEN, "attr_b", StorageType.INT_MEMBER, "int16_t"),
            (READ_GEN, "attr_c", StorageType.INT_MEMBER, "int16_t"),
            (READ_GEN, "attr_d", StorageType.FLOAT_MEMBER, "float"),
        ]

        return data_format


class EffectBundle(GenieStructure):  # also called techage in some other tools

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        if game_version[0].game_id in ("AOE1DE", "AOE2DE"):
            data_format = [
                (SKIP, "name_len_debug", StorageType.INT_MEMBER, "uint16_t"),
                (READ, "name_len", StorageType.INT_MEMBER, "uint16_t"),
                (READ_GEN, "name", StorageType.STRING_MEMBER, "char[name_len]"),
            ]
        else:
            data_format = [
                # always CHUN4 (change unit 4-arg) in AoE1-AoC, later versions name them
                (READ_GEN, "name", StorageType.STRING_MEMBER, "char[31]"),
            ]

        data_format.extend([
            (READ, "effect_count", StorageType.INT_MEMBER, "uint16_t"),
            (READ_GEN, "effects", StorageType.ARRAY_CONTAINER, SubdataMember(
                ref_type=Effect,
                length="effect_count",
            )),
        ])

        return data_format


class OtherConnection(GenieStructure):

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, "other_connection", StorageType.ID_MEMBER, EnumLookupMember(  # mode for unit_or_research0
                raw_type="int32_t",
                type_name="connection_mode",
                lookup_dict=CONNECTION_MODE
            )),
        ]

        return data_format


class AgeTechTree(GenieStructure):

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, "id", StorageType.ID_MEMBER, "int32_t"),
            # 0=generic
            # 1=TODO
            # 2=default
            # 3=marks as not available
            # 4=upgrading, constructing, creating
            # 5=research completed, building built
            (READ_GEN, "status", StorageType.ID_MEMBER, "int8_t"),
        ]

        if game_version[0].game_id != "ROR":
            data_format.extend([
                (READ, "building_count", StorageType.INT_MEMBER, "uint8_t"),
                (READ_GEN, "buildings", StorageType.ARRAY_ID, "int32_t[building_count]"),
                (READ, "unit_count", StorageType.INT_MEMBER, "uint8_t"),
                (READ_GEN, "units", StorageType.ARRAY_ID, "int32_t[unit_count]"),
                (READ, "research_count", StorageType.INT_MEMBER, "uint8_t"),
                (READ_GEN, "researches", StorageType.ARRAY_ID, "int32_t[research_count]"),
            ])
        else:
            data_format.extend([
                (READ, "building_count", StorageType.INT_MEMBER, "uint8_t"),
                (READ_GEN, "buildings", StorageType.ARRAY_ID, "int32_t[40]"),
                (READ, "unit_count", StorageType.INT_MEMBER, "uint8_t"),
                (READ_GEN, "units", StorageType.ARRAY_ID, "int32_t[40]"),
                (READ, "research_count", StorageType.INT_MEMBER, "uint8_t"),
                (READ_GEN, "researches", StorageType.ARRAY_ID, "int32_t[40]"),
            ])

        data_format.extend([
            (READ_GEN, "connected_slots_used", StorageType.INT_MEMBER, "int32_t"),
        ])

        if game_version[0].game_id == "SWGB":
            data_format.extend([
                (READ_GEN, "other_connected_ids", StorageType.ARRAY_ID, "int32_t[20]"),
                (READ_GEN, "other_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
                    ref_type=OtherConnection,
                    length=20,
                )),
            ])
        elif game_version[0].game_id == "ROR":
            data_format.extend([
                (READ_GEN, "other_connected_ids", StorageType.ARRAY_ID, "int32_t[5]"),
                (READ_GEN, "other_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
                    ref_type=OtherConnection,
                    length=5,
                )),
            ])
        else:
            data_format.extend([
                (READ_GEN, "other_connected_ids", StorageType.ARRAY_ID, "int32_t[10]"),
                (READ_GEN, "other_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
                    ref_type=OtherConnection,
                    length=10,
                )),
            ])

        data_format.extend([
            (READ_GEN, "building_level_count", StorageType.INT_MEMBER, "int8_t"),
        ])

        if game_version[0].game_id == "SWGB":
            data_format.extend([
                (READ_GEN, "buildings_per_zone", StorageType.ARRAY_INT, "int8_t[20]"),
                (READ_GEN, "group_length_per_zone", StorageType.ARRAY_INT, "int8_t[20]"),
            ])
        elif game_version[0].game_id == "ROR":
            data_format.extend([
                (READ_GEN, "buildings_per_zone", StorageType.ARRAY_INT, "int8_t[3]"),
                (READ_GEN, "group_length_per_zone", StorageType.ARRAY_INT, "int8_t[3]"),
            ])
        else:
            data_format.extend([
                (READ_GEN, "buildings_per_zone", StorageType.ARRAY_INT, "int8_t[10]"),
                (READ_GEN, "group_length_per_zone", StorageType.ARRAY_INT, "int8_t[10]"),
            ])

        data_format.extend([
            (READ_GEN, "max_age_length", StorageType.INT_MEMBER, "int8_t"),
            # 1= Age
            (READ_GEN, "line_mode", StorageType.ID_MEMBER, "int32_t"),
        ])

        return data_format


class BuildingConnection(GenieStructure):

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            # unit id of the current building
            (READ_GEN, "id", StorageType.ID_MEMBER, "int32_t"),
            # 0=generic
            # 1=TODO
            # 2=default
            # 3=marks as not available
            # 4=upgrading, constructing, creating
            # 5=research completed, building built
            # maybe always 2 because we got 2 of them hardcoded below
            # (unit_or_research, mode)
            (READ, "status", StorageType.ID_MEMBER, "int8_t"),
        ]

        if game_version[0].game_id != "ROR":
            data_format.extend([
                (READ, "building_count", StorageType.INT_MEMBER, "uint8_t"),
                # new buildings available when this building was created
                (READ_GEN, "buildings", StorageType.ARRAY_ID, "int32_t[building_count]"),
                (READ, "unit_count", StorageType.INT_MEMBER, "uint8_t"),
                # new units
                (READ_GEN, "units", StorageType.ARRAY_ID, "int32_t[unit_count]"),
                (READ, "research_count", StorageType.INT_MEMBER, "uint8_t"),
                # new researches
                (READ_GEN, "researches", StorageType.ARRAY_ID, "int32_t[research_count]"),
            ])
        else:
            data_format.extend([
                (READ, "building_count", StorageType.INT_MEMBER, "uint8_t"),
                (READ_GEN, "buildings", StorageType.ARRAY_ID, "int32_t[40]"),
                (READ, "unit_count", StorageType.INT_MEMBER, "uint8_t"),
                (READ_GEN, "units", StorageType.ARRAY_ID, "int32_t[40]"),
                (READ, "research_count", StorageType.INT_MEMBER, "uint8_t"),
                (READ_GEN, "researches", StorageType.ARRAY_ID, "int32_t[40]"),
            ])

        data_format.extend([
            (READ_GEN, "connected_slots_used", StorageType.INT_MEMBER, "int32_t"),
        ])

        if game_version[0].game_id == "SWGB":
            data_format.extend([
                (READ_GEN, "other_connected_ids", StorageType.ARRAY_ID, "int32_t[20]"),
                (READ_GEN, "other_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
                    ref_type=OtherConnection,
                    length=20,
                )),
            ])
        elif game_version[0].game_id == "ROR":
            data_format.extend([
                (READ_GEN, "other_connected_ids", StorageType.ARRAY_ID, "int32_t[5]"),
                (READ_GEN, "other_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
                    ref_type=OtherConnection,
                    length=5,
                )),
            ])
        else:
            data_format.extend([
                (READ_GEN, "other_connected_ids", StorageType.ARRAY_ID, "int32_t[10]"),
                (READ_GEN, "other_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
                    ref_type=OtherConnection,
                    length=10,
                )),
            ])

        data_format.extend([
            # minimum age, in which this building is available
            (READ_GEN, "location_in_age", StorageType.ID_MEMBER, "int8_t"),
            # total techs for each age (5 ages, post-imp probably counts as one)
            (READ_GEN, "unit_techs_total", StorageType.ARRAY_INT, "int8_t[5]"),
            (READ_GEN, "unit_techs_first", StorageType.ARRAY_INT, "int8_t[5]"),
            # 5: >=1 connections, 6: no connections
            (READ_GEN, "line_mode", StorageType.ID_MEMBER, "int32_t"),
            # current building is unlocked by this research id, -1=no unlock needed
            (READ_GEN, "enabling_research", StorageType.ID_MEMBER, "int32_t"),
        ])

        return data_format


class UnitConnection(GenieStructure):

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, "id", StorageType.ID_MEMBER, "int32_t"),
            # 0=generic
            # 1=TODO
            # 2=default
            # 3=marks as not available
            # 4=upgrading, constructing, creating
            # 5=research completed, building built
            (READ_GEN, "status", StorageType.ID_MEMBER, "int8_t"),  # always 2: default
            # building, where this unit is created
            (READ_GEN, "upper_building", StorageType.ID_MEMBER, "int32_t"),
            (READ_GEN, "connected_slots_used", StorageType.INT_MEMBER, "int32_t"),
        ]

        if game_version[0].game_id == "SWGB":
            data_format.extend([
                (READ_GEN, "other_connected_ids", StorageType.ARRAY_ID, "int32_t[20]"),
                (READ_GEN, "other_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
                    ref_type=OtherConnection,
                    length=20,
                )),
            ])
        elif game_version[0].game_id == "ROR":
            data_format.extend([
                (READ_GEN, "other_connected_ids", StorageType.ARRAY_ID, "int32_t[5]"),
                (READ_GEN, "other_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
                    ref_type=OtherConnection,
                    length=5,
                )),
            ])
        else:
            data_format.extend([
                (READ_GEN, "other_connected_ids", StorageType.ARRAY_ID, "int32_t[10]"),
                (READ_GEN, "other_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
                    ref_type=OtherConnection,
                    length=10,
                )),
            ])

        data_format.extend([
            (READ_GEN, "vertical_line", StorageType.ID_MEMBER, "int32_t"),
        ])

        if game_version[0].game_id != "ROR":
            data_format.extend([
                (READ, "unit_count", StorageType.INT_MEMBER, "uint8_t"),
                (READ_GEN, "units", StorageType.ARRAY_ID, "int32_t[unit_count]"),
            ])
        else:
            data_format.extend([
                (READ, "unit_count", StorageType.INT_MEMBER, "uint8_t"),
                (READ_GEN, "units", StorageType.ARRAY_ID, "int32_t[40]"),
            ])

        data_format.extend([
            (READ_GEN, "location_in_age", StorageType.ID_MEMBER, "int32_t"),    # 0=hidden, 1=first, 2=second
            # min amount of researches to be discovered for this unit to be
            # available
            (READ_GEN, "required_research", StorageType.ID_MEMBER, "int32_t"),
            # 2=first unit in line
            # 3=unit that depends on a previous research in its line
            (READ_GEN, "line_mode", StorageType.ID_MEMBER, "int32_t"),
            (READ_GEN, "enabling_research", StorageType.ID_MEMBER, "int32_t"),
        ])

        return data_format


class ResearchConnection(GenieStructure):

    @classmethod
    def get_data_format_members(cls, game_version):
        """
        Return the members in this struct.
        """
        data_format = [
            (READ_GEN, "id", StorageType.ID_MEMBER, "int32_t"),
            # 0=generic
            # 1=TODO
            # 2=default
            # 3=marks as not available
            # 4=upgrading, constructing, creating
            # 5=research completed, building built
            (READ_GEN, "status", StorageType.ID_MEMBER, "int8_t"),
            (READ_GEN, "upper_building", StorageType.ID_MEMBER, "int32_t"),
        ]

        if game_version[0].game_id != "ROR":
            data_format.extend([
                (READ, "building_count", StorageType.INT_MEMBER, "uint8_t"),
                (READ_GEN, "buildings", StorageType.ARRAY_ID, "int32_t[building_count]"),
                (READ, "unit_count", StorageType.INT_MEMBER, "uint8_t"),
                (READ_GEN, "units", StorageType.ARRAY_ID, "int32_t[unit_count]"),
                (READ, "research_count", StorageType.INT_MEMBER, "uint8_t"),
                (READ_GEN, "researches", StorageType.ARRAY_ID, "int32_t[research_count]"),
            ])
        else:
            data_format.extend([
                (READ, "building_count", StorageType.INT_MEMBER, "uint8_t"),
                (READ_GEN, "buildings", StorageType.ARRAY_ID, "int32_t[40]"),
                (READ, "unit_count", StorageType.INT_MEMBER, "uint8_t"),
                (READ_GEN, "units", StorageType.ARRAY_ID, "int32_t[40]"),
                (READ, "research_count", StorageType.INT_MEMBER, "uint8_t"),
                (READ_GEN, "researches", StorageType.ARRAY_ID, "int32_t[40]"),
            ])

        data_format.extend([
            (READ_GEN, "connected_slots_used", StorageType.INT_MEMBER, "int32_t"),
        ])

        if game_version[0].game_id == "SWGB":
            data_format.extend([
                (READ_GEN, "other_connected_ids", StorageType.ARRAY_ID, "int32_t[20]"),
                (READ_GEN, "other_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
                    ref_type=OtherConnection,
                    length=20,
                )),
            ])
        elif game_version[0].game_id == "ROR":
            data_format.extend([
                (READ_GEN, "other_connected_ids", StorageType.ARRAY_ID, "int32_t[5]"),
                (READ_GEN, "other_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
                    ref_type=OtherConnection,
                    length=5,
                )),
            ])
        else:
            data_format.extend([
                (READ_GEN, "other_connected_ids", StorageType.ARRAY_ID, "int32_t[10]"),
                (READ_GEN, "other_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
                    ref_type=OtherConnection,
                    length=10,
                )),
            ])

        data_format.extend([
            (READ_GEN, "vertical_line", StorageType.ID_MEMBER, "int32_t"),
            (READ_GEN, "location_in_age", StorageType.ID_MEMBER, "int32_t"),    # 0=hidden, 1=first, 2=second
            # 0=first age unlocks
            # 4=research
            (READ_GEN, "line_mode", StorageType.ID_MEMBER, "int32_t"),
        ])

        return data_format

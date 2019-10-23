# Copyright 2013-2019 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from ..dataformat.genie_structure import GenieStructure
from ..dataformat.read_members import SubdataMember, EnumLookupMember
from ..dataformat.member_access import READ, READ_EXPORT
from ..dataformat.value_members import MemberTypes as StorageType


class Effect(GenieStructure):
    name_struct = "tech_effect"
    name_struct_file = "tech"
    struct_description = "applied effect for a research technology."

    data_format = [
        (READ, "type_id", StorageType.ID_MEMBER, EnumLookupMember(
            raw_type="int8_t",
            type_name="effect_apply_type",
            lookup_dict={
                # unused assignage: a = -1, b = -1, c = -1, d = 0
                -1: "DISABLED",
                # if a != -1: a == unit_id, else b == unit_class_id; c =
                # attribute_id, d = new_value
                0: "ATTRIBUTE_ABSSET",
                # a == resource_id, if b == 0: then d = absval, else d = relval
                # (for inc/dec)
                1: "RESOURCE_MODIFY",
                # a == unit_id, if b == 0: disable unit, else b == 1: enable
                # unit
                2: "UNIT_ENABLED",
                # a == old_unit_id, b == new_unit_id
                3: "UNIT_UPGRADE",
                # if a != -1: unit_id, else b == unit_class_id; c=attribute_id,
                # d=relval
                4: "ATTRIBUTE_RELSET",
                # if a != -1: unit_id, else b == unit_class_id; c=attribute_id,
                # d=factor
                5: "ATTRIBUTE_MUL",
                # a == resource_id, d == factor
                6: "RESOURCE_MUL",

                # may mean something different in aok:hd:
                10: "TEAM_ATTRIBUTE_ABSSET",
                11: "TEAM_RESOURCE_MODIFY",
                12: "TEAM_UNIT_ENABLED",
                13: "TEAM_UNIT_UPGRADE",
                14: "TEAM_ATTRIBUTE_RELSET",
                15: "TEAM_ATTRIBUTE_MUL",
                16: "TEAM_RESOURCE_MUL",

                # these are only used in technology trees, 103 even requires
                # one
                # a == research_id, b == resource_id, if c == 0: d==absval
                # else: d == relval
                101: "TECHCOST_MODIFY",
                102: "TECH_TOGGLE",       # d == research_id
                103: "TECH_TIME_MODIFY",  # a == research_id, if c == 0: d==absval else d==relval

                # attribute_id:
                # 0: hit points
                # 1: line of sight
                # 2: garrison capacity
                # 3: unit size x
                # 4: unit size y
                # 5: movement speed
                # 6: rotation speed
                # 7: unknown
                # 8: armor                           # real_val = val + (256 * armor_id)
                # 9: attack                          # real_val = val + (256 * attack_id)
                # 10: attack reloading time
                # 11: accuracy percent
                # 12: max range
                # 13: working rate
                # 14: resource carriage
                # 15: default armor
                # 16: projectile unit
                # 17: upgrade graphic (icon), graphics angle
                # 18: terrain restriction to multiply damage received (always sets)
                # 19: intelligent projectile aim 1=on, 0=off
                # 20: minimum range
                # 21: first resource storage
                # 22: blast width (area damage)
                # 23: search radius
                # 80: boarding energy reload speed
                # 100: resource cost
                # 101: creation time
                # 102: number of garrison arrows
                # 103: food cost
                # 104: wood cost
                # 105: stone cost
                # 106: gold cost
                # 107: max total projectiles
                # 108: garrison healing rate
                # 109: regeneration rate
            },
        )),
        (READ, "attr_a", StorageType.INT_MEMBER, "int16_t"),
        (READ, "attr_b", StorageType.INT_MEMBER, "int16_t"),
        (READ, "attr_c", StorageType.INT_MEMBER, "int16_t"),
        (READ, "attr_d", StorageType.FLOAT_MEMBER, "float"),
    ]


class EffectBundle(GenieStructure):  # also called techage in some other tools
    name_struct = "effect_bundle"
    name_struct_file = "tech"
    struct_description = "a bundle of effects."

    data_format = [
        # always CHUN4 (change unit 4-arg) in AoE1-AoC, later versions name them
        (READ, "name", StorageType.STRING_MEMBER, "char[31]"),
        (READ, "effect_count", StorageType.INT_MEMBER, "uint16_t"),
        (READ, "effects", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=Effect,
            length="effect_count",
        )),
    ]


# TODO: add common tech class

class OtherConnection(GenieStructure):
    name_struct = "other_connection"
    name_struct_file = "tech"
    struct_description = "misc connection for a building/unit/research connection"

    data_format = [
        (READ, "other_connection", StorageType.ID_MEMBER, EnumLookupMember(  # mode for unit_or_research0
            raw_type="int32_t",
            type_name="connection_mode",
            lookup_dict={
                0: "NOTHING",
                1: "BUILDING",
                2: "UNIT",
                3: "RESEARCH",
            }
        )),
    ]


class AgeTechTree(GenieStructure):
    name_struct = "age_tech_tree"
    name_struct_file = "tech"
    struct_description = "items available when this age was reached."

    data_format = [
        (READ, "id", StorageType.ID_MEMBER, "int32_t"),
        # 0=generic
        # 1=TODO
        # 2=default
        # 3=marks as not available
        # 4=upgrading, constructing, creating
        # 5=research completed, building built
        (READ, "status", StorageType.ID_MEMBER, "int8_t"),
    ]

    # TODO: Enable conversion for AOE1; replace 6 values below
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.extend([
    #         (READ, "building_count", "int8_t"),
    #         (READ, "buildings", "int32_t[building_count]"),
    #         (READ, "unit_count", "int8_t"),
    #         (READ, "units", "int32_t[unit_count]"),
    #         (READ, "research_count", "int8_t"),
    #         (READ, "researches", "int32_t[research_count]"),
    #     ])
    # else:
    #     data_format.extend([
    #         (READ, "building_count", "int8_t"),
    #         (READ, "buildings", "int32_t[40]"),
    #         (READ, "unit_count", "int8_t"),
    #         (READ, "units", "int32_t[40]"),
    #         (READ, "research_count", "int8_t"),
    #         (READ, "researches", "int32_t[40]"),
    #     ])
    # ===========================================================================
    data_format.extend([
        (READ, "building_count", StorageType.INT_MEMBER, "int8_t"),
        (READ, "buildings", StorageType.ARRAY_ID, "int32_t[building_count]"),
        (READ, "unit_count", StorageType.INT_MEMBER, "int8_t"),
        (READ, "units", StorageType.ARRAY_ID, "int32_t[unit_count]"),
        (READ, "research_count", StorageType.INT_MEMBER, "int8_t"),
        (READ, "researches", StorageType.ARRAY_ID, "int32_t[research_count]"),
    ])
    # ===========================================================================

    data_format.extend([
        (READ, "slots_used", StorageType.INT_MEMBER, "int32_t"),
        (READ, "unit_researches", StorageType.ARRAY_ID, "int32_t[10]"),
        (READ, "other_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=OtherConnection,
            length=10,
        )),

        (READ, "building_level_count", StorageType.INT_MEMBER, "int8_t"),
    ])

    # TODO: Enable conversion for SWGB; replace "buildings_per_zone", "group_length_per_zone"
    # ===========================================================================
    # if (GameVersion.swgb_10 or GameVersion.swgb_cc) in game_versions:
    #     data_format.extend([
    #         (READ, "buildings_per_zone", "int8_t[20]"),
    #         (READ, "group_length_per_zone", "int8_t[20]"),
    #     ])
    # else:
    #     data_format.extend([
    #         (READ, "buildings_per_zone", "int8_t[10]"),
    #         (READ, "group_length_per_zone", "int8_t[10]"),
    #     ])
    # ===========================================================================
    data_format.extend([
        (READ, "buildings_per_zone", StorageType.ARRAY_INT, "int8_t[10]"),
        (READ, "group_length_per_zone", StorageType.ARRAY_INT, "int8_t[10]"),
    ])

    data_format.extend([
        (READ, "max_age_length", StorageType.INT_MEMBER, "int8_t"),
        # 1= Age
        (READ, "line_mode", StorageType.ID_MEMBER, "int32_t"),
    ])


class BuildingConnection(GenieStructure):
    name_struct = "building_connection"
    name_struct_file = "tech"
    struct_description = "new available buildings/units/researches when this building was created."

    data_format = [
        # unit id of the current building
        (READ_EXPORT, "id", StorageType.ID_MEMBER, "int32_t"),
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

    # TODO: Enable conversion for AOE1; replace 6 values below
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.extend([
    #         (READ_EXPORT, "building_count", "int8_t"),
    #         (READ, "buildings", "int32_t[building_count]"),   # new buildings available when this building was created
    #         (READ_EXPORT, "unit_count", "int8_t"),
    #         (READ, "units", "int32_t[unit_count]"),           # new units
    #         (READ_EXPORT, "research_count", "int8_t"),
    #         (READ, "researches", "int32_t[research_count]"),  # new researches
    #     ])
    # else:
    #     data_format.extend([
    #         (READ_EXPORT, "building_count", "int8_t"),
    #         (READ, "buildings", "int32_t[40]"),
    #         (READ_EXPORT, "unit_count", "int8_t"),
    #         (READ, "units", "int32_t[40]"),
    #         (READ_EXPORT, "research_count", "int8_t"),
    #         (READ, "researches", "int32_t[40]"),
    #     ])
    # ===========================================================================
    data_format.extend([
        (READ_EXPORT, "building_count", StorageType.INT_MEMBER, "int8_t"),
        # new buildings available when this building was created
        (READ, "buildings", StorageType.ARRAY_ID, "int32_t[building_count]"),
        (READ_EXPORT, "unit_count", StorageType.INT_MEMBER, "int8_t"),
        (READ, "units", StorageType.ARRAY_ID, "int32_t[unit_count]"),           # new units
        (READ_EXPORT, "research_count", StorageType.INT_MEMBER, "int8_t"),
        (READ, "researches", StorageType.ARRAY_ID, "int32_t[research_count]"),  # new researches
    ])
    # ===========================================================================

    data_format.extend([
        (READ, "slots_used", StorageType.INT_MEMBER, "int32_t"),
        (READ, "unit_researches", StorageType.ARRAY_ID, "int32_t[10]"),
        (READ, "other_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=OtherConnection,
            length=10,
        )),

        # minimum age, in which this building is available
        (READ, "location_in_age", StorageType.ID_MEMBER, "int8_t"),
        # total techs for each age (5 ages, post-imp probably counts as one)
        (READ, "unit_techs_total", StorageType.ARRAY_INT, "int8_t[5]"),
        (READ, "unit_techs_first", StorageType.ARRAY_INT, "int8_t[5]"),
        # 5: >=1 connections, 6: no connections
        (READ_EXPORT, "line_mode", StorageType.ID_MEMBER, "int32_t"),
        # current building is unlocked by this research id, -1=no unlock needed
        (READ, "enabled_by_research_id", StorageType.ID_MEMBER, "int32_t"),
    ])


class UnitConnection(GenieStructure):
    name_struct = "unit_connection"
    name_struct_file = "tech"
    struct_description = "unit updates to apply when activating the technology."

    data_format = [
        (READ, "id", StorageType.ID_MEMBER, "int32_t"),
        # 0=generic
        # 1=TODO
        # 2=default
        # 3=marks as not available
        # 4=upgrading, constructing, creating
        # 5=research completed, building built
        (READ, "status", StorageType.ID_MEMBER, "int8_t"),  # always 2: default
        # building, where this unit is created
        (READ, "upper_building", StorageType.ID_MEMBER, "int32_t"),

        (READ, "slots_used", StorageType.INT_MEMBER, "int32_t"),
        (READ, "unit_researches", StorageType.ARRAY_ID, "int32_t[10]"),
        (READ, "other_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=OtherConnection,
            length=10,
        )),

        (READ, "vertical_line", StorageType.ID_MEMBER, "int32_t"),
    ]

    # TODO: Enable conversion for AOE1; replace "unit_count", "units"
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.extend([
    #         (READ, "unit_count", "int8_t"),
    #         (READ, "units", "int32_t[unit_count]"),
    #     ])
    # else:
    #     data_format.extend([
    #         (READ, "unit_count", "int8_t"),
    #         (READ, "units", "int32_t[40]"),
    #     ])
    # ===========================================================================
    data_format.extend([
        (READ, "unit_count", StorageType.INT_MEMBER, "int8_t"),
        (READ, "units", StorageType.ARRAY_ID, "int32_t[unit_count]"),
    ])

    data_format.extend([
        (READ, "location_in_age", StorageType.ID_MEMBER, "int32_t"),    # 0=hidden, 1=first, 2=second
        # min amount of researches to be discovered for this unit to be
        # available
        (READ, "required_research", StorageType.ID_MEMBER, "int32_t"),
        # 2=first unit in line
        # 3=unit that depends on a previous research in its line
        (READ, "line_mode", StorageType.ID_MEMBER, "int32_t"),
        (READ, "enabling_research", StorageType.ID_MEMBER, "int32_t"),
    ])


class ResearchConnection(GenieStructure):
    name_struct = "research_connection"
    name_struct_file = "tech"
    struct_description = "research updates to apply when activating the technology."

    data_format = [
        (READ, "id", StorageType.ID_MEMBER, "int32_t"),
        # 0=generic
        # 1=TODO
        # 2=default
        # 3=marks as not available
        # 4=upgrading, constructing, creating
        # 5=research completed, building built
        (READ, "status", StorageType.ID_MEMBER, "int8_t"),
        (READ, "upper_building", StorageType.ID_MEMBER, "int32_t"),
    ]

    # TODO: Enable conversion for AOE1; replace 6 values below
    # ===========================================================================
    # if (GameVersion.aoe_1 or GameVersion.aoe_ror) not in game_versions:
    #     data_format.extend([
    #         (READ, "building_count", "int8_t"),
    #         (READ, "buildings", "int32_t[building_count]"),
    #         (READ, "unit_count", "int8_t"),
    #         (READ, "units", "int32_t[unit_count]"),
    #         (READ, "research_count", "int8_t"),
    #         (READ, "researches", "int32_t[research_count]"),
    #     ])
    # else:
    #     data_format.extend([
    #         (READ, "building_count", "int8_t"),
    #         (READ, "buildings", "int32_t[40]"),
    #         (READ, "unit_count", "int8_t"),
    #         (READ, "units", "int32_t[40]"),
    #         (READ, "research_count", "int8_t"),
    #         (READ, "researches", "int32_t[40]"),
    #     ])
    # ===========================================================================
    data_format.extend([
        (READ, "building_count", StorageType.INT_MEMBER, "int8_t"),
        (READ, "buildings", StorageType.ARRAY_ID, "int32_t[building_count]"),
        (READ, "unit_count", StorageType.INT_MEMBER, "int8_t"),
        (READ, "units", StorageType.ARRAY_ID, "int32_t[unit_count]"),
        (READ, "research_count", StorageType.INT_MEMBER, "int8_t"),
        (READ, "researches", StorageType.ARRAY_ID, "int32_t[research_count]"),
    ])
    # ===========================================================================

    data_format.extend([
        (READ, "slots_used", StorageType.INT_MEMBER, "int32_t"),
        (READ, "unit_researches", StorageType.ARRAY_ID, "int32_t[10]"),
        (READ, "other_connections", StorageType.ARRAY_CONTAINER, SubdataMember(
            ref_type=OtherConnection,
            length=10,
        )),

        (READ, "vertical_line", StorageType.ID_MEMBER, "int32_t"),
        (READ, "location_in_age", StorageType.ID_MEMBER, "int32_t"),    # 0=hidden, 1=first, 2=second
        # 0=first age unlocks
        # 4=research
        (READ, "line_mode", StorageType.ID_MEMBER, "int32_t"),
    ])

# Copyright 2013-2015 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from ..dataformat.exportable import Exportable
from ..dataformat.members import SubdataMember, EnumLookupMember, ZeroMember
from ..dataformat.member_access import READ, READ_EXPORT, READ_UNKNOWN


class Effect(Exportable):
    name_struct        = "tech_effect"
    name_struct_file   = "tech"
    struct_description = "applied effect for a research technology."

    data_format = (
        (READ, "type_id",   EnumLookupMember(
            raw_type = "int8_t",
            type_name = "effect_apply_type",
            lookup_dict = {
                # unused assignage: a = -1, b = -1, c = -1, d = 0
                -1: "DISABLED",
                0: "ATTRIBUTE_ABSSET",    # if a != -1: a == unit_id, else b == unit_class_id; c = attribute_id, d = new_value
                1: "RESOURCE_MODIFY",     # a == resource_id, if b == 0: then d = absval, else d = relval (for inc/dec)
                2: "UNIT_ENABLED",        # a == unit_id, if b == 0: disable unit, else b == 1: enable unit
                3: "UNIT_UPGRADE",        # a == old_unit_id, b == new_unit_id
                4: "ATTRIBUTE_RELSET",    # if a != -1: unit_id, else b == unit_class_id; c=attribute_id, d=relval
                5: "ATTRIBUTE_MUL",       # if a != -1: unit_id, else b == unit_class_id; c=attribute_id, d=factor
                6: "RESOURCE_MUL",        # a == resource_id, d == factor

                # these are only used in technology trees, 103 even requires one
                101: "TECHCOST_MODIFY",   # a == research_id, b == resource_id, if c == 0: d==absval else: d == relval
                102: "TECH_DISABLE",      # d == research_id
                103: "TECH_TIME_MODIFY",  # a == research_id, if c == 0: d==absval else d==relval

                # attribute_id:
                # 0: hit points
                # 1: line of sight
                # 2: garrison capacity
                # 3: unit size radius 0
                # 4: unit size radius 1
                # 5: movement speed
                # 6: garrison recovery rate
                # 7: unknown
                # 8: armor                           # real_val = val + (256 * armor_id)
                # 9: attack                          # real_val = val + (256 * attack_id)
                # 10: attack reloading time
                # 11: accuracy percent
                # 12: range
                # 13: working rate
                # 14: resource carriage
                # 15: default armor
                # 16: new projectile unit
                # 17: upgrade graphic (icon), graphics angle
                # 18: terrain restriction damage TODO
                # 19: intelligent projectile aim 1=on, 0=off
                # 20: minimum range
                # 21: population support
                # 22: blast radius (area damage)
                # 23: search radius
                # 80: boarding energy reload speed
                # 100: resource cost
                # 101: creation time
                # 102: number of garrison arrows
                # 103: food cost
                # 104: wood cost
                # 105: stone cost
                # 106: gold cost
                # 107: max duplicated missiles
                # 108: healing rate
            },
        )),
        (READ, "unit",      "int16_t"),       # == a
        (READ, "unit_class_id",  "int16_t"),  # == b
        (READ, "attribute_id", "int16_t"),    # == c
        (READ, "amount",    "float"),         # == d
    )


class Tech(Exportable):  # also called techage in some other tools
    name_struct        = "tech"
    name_struct_file   = "tech"
    struct_description = "a technology definition."

    data_format = (
        (READ, "name", "char[31]"),                  # always CHUN4 (change unit 4-arg)
        (READ, "effect_count", "uint16_t"),
        (READ, "effects", SubdataMember(
            ref_type=Effect,
            length="effect_count",
        )),
    )


# TODO: add common tech class


class AgeTechTree(Exportable):
    name_struct        = "age_tech_tree"
    name_struct_file   = "tech"
    struct_description = "items available when this age was reached."

    data_format = (
        (READ_UNKNOWN, None, "int32_t"),
        (READ, "id", "int32_t"),
        (READ_UNKNOWN, None, "int8_t"),
        (READ, "building_count", "int8_t"),
        (READ, "buildings", "int32_t[building_count]"),
        (READ, "unit_count", "int8_t"),
        (READ, "units", "int32_t[unit_count]"),
        (READ, "research_count", "int8_t"),
        (READ, "researches", "int32_t[research_count]"),
        (READ_UNKNOWN, None, "int32_t"),                  # always 1
        (READ, "second_age_id", "int32_t"),
        (READ, "zeroes", ZeroMember("int16_t", length=49)),
    )


class BuildingConnection(Exportable):
    name_struct        = "building_connection"
    name_struct_file   = "tech"
    struct_description = "new available buildings/units/researches when this building was created."

    data_format = (
        (READ_EXPORT, "id", "int32_t"),                   # unit id of the current building
        (READ, "prerequisite_count", "int8_t"),           # always 2, as we got 2 of them hardcoded below (unit_or_research, mode)
        (READ_EXPORT, "building_count", "int8_t"),
        (READ, "buildings", "int32_t[building_count]"),   # new buildings available when this building was created
        (READ_EXPORT, "unit_count", "int8_t"),
        (READ, "units", "int32_t[unit_count]"),           # new units
        (READ_EXPORT, "research_count", "int8_t"),
        (READ, "researches", "int32_t[research_count]"),  # new researches
        (READ_EXPORT, "age", "int32_t"),                  # minimum age, in which this building is available
        (READ, "unit_or_research0", "int32_t"),           # this building depends on research_id or unit_id, which is set in mode0
        (READ, "unit_or_research1", "int32_t"),           # dito, set in mode1
        (READ_UNKNOWN, None, "int32_t[8]"),
        (READ, "mode0", EnumLookupMember(                 # mode for unit_or_research0
            raw_type = "int32_t",
            type_name = "building_connection_mode",
            lookup_dict = {
                0: "NOTHING",
                1: "BUILDING",
                2: "UNIT",
                3: "RESEARCH",
            }
        )),
        (READ, "mode1", "int32_t"),  # TODO, xref possible values to the enum above (reuse them)
        (READ_UNKNOWN, None, "int32_t[8]"),
        (READ_UNKNOWN, None, "int8_t[11]"),
        (READ_EXPORT, "line_mode", "int32_t"),          # 5: >=1 connections, 6: no connections
        (READ, "enabled_by_research_id", "int32_t"),      # current building is unlocked by this research id, -1=no unlock needed
    )


class UnitConnection(Exportable):
    name_struct        = "unit_connection"
    name_struct_file   = "tech"
    struct_description = "unit updates to apply when activating the technology."

    data_format = (
        (READ, "id", "int32_t"),
        (READ_UNKNOWN, None, "int8_t"),
        (READ, "upper_building", "int32_t"),        # building, where this unit is created
        (READ, "required_researches", "int32_t"),
        (READ, "age", "int32_t"),
        (READ, "unit_or_research0", "int32_t"),
        (READ, "unit_or_research1", "int32_t"),
        (READ_UNKNOWN, None, "int32_t[8]"),
        (READ, "mode0", "int32_t"),
        (READ, "mode1", "int32_t"),
        (READ_UNKNOWN, None, "int32_t[7]"),
        (READ, "vertical_lines", "int32_t"),
        (READ, "unit_count", "int8_t"),
        (READ, "units", "int32_t[unit_count]"),
        (READ, "location_in_age", "int32_t"),    # 0=hidden, 1=first, 2=second
        (READ, "required_research", "int32_t"),  # min amount of researches to be discovered for this unit to be available
        (READ, "line_mode", "int32_t"),          # 0=independent/new in its line, 3=depends on a previous research in its line
        (READ, "enabling_research", "int32_t"),
    )


class ResearchConnection(Exportable):
    name_struct        = "research_connection"
    name_struct_file   = "tech"
    struct_description = "research updates to apply when activating the technology."

    data_format = (
        (READ, "id", "int32_t"),
        (READ_UNKNOWN, None, "int8_t"),
        (READ, "upper_building", "int32_t"),
        (READ, "building_count", "int8_t"),
        (READ, "buildings", "int32_t[building_count]"),
        (READ, "unit_count", "int8_t"),
        (READ, "units", "int32_t[unit_count]"),
        (READ, "research_count", "int8_t"),
        (READ, "researches", "int32_t[research_count]"),
        (READ, "required_research", "int32_t"),
        (READ, "age", "int32_t"),
        (READ, "upper_research", "int32_t"),
        (READ_UNKNOWN, None, "int32_t[9]"),
        (READ, "line_mode", "int32_t"),
        (READ_UNKNOWN, None, "int32_t[8]"),
        (READ, "vertical_line", "int32_t"),
        (READ, "location_in_age", "int32_t"),  # 0=hidden, 1=first, 2=second
        (READ, "line_mode", "int32_t"),        # 0=first age, else other ages.
    )

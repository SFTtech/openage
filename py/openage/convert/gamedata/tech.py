# Copyright 2013-2014 the openage authors. See copying.md for legal info.

from .. import dataformat
from struct import Struct, unpack_from
from ..util import dbg, zstr, offset_info

from .empiresdat import endianness


class Effect(dataformat.Exportable):
    name_struct        = "tech_effect"
    name_struct_file   = "tech"
    struct_description = "applied effect for a research technology."

    data_format = (
        (dataformat.READ, "type_id",   dataformat.EnumLookupMember(
            raw_type = "int8_t",
            type_name = "effect_apply_type",
            lookup_dict = {
                #unused assignage: a = -1, b = -1, c = -1, d = 0
                -1: "DISABLED",
                0: "ATTRIBUTE_ABSSET",   #if a != -1: a == unit_id, else b == unit_class_id; c = attribute_id, d = new_value
                1: "RESOURCE_MODIFY",   #a == resource_id, if b == 0: then d = absval, else d = relval (for inc/dec)
                2: "UNIT_ENABLED",       #a == unit_id, if b == 0: disable unit, else b == 1: enable unit
                3: "UNIT_UPGRADE",       #a == old_unit_id, b == new_unit_id
                4: "ATTRIBUTE_RELSET",   #if a != -1: unit_id, else b == unit_class_id; c=attribute_id, d=relval
                5: "ATTRIBUTE_MUL",      #if a != -1: unit_id, else b == unit_class_id; c=attribute_id, d=factor
                6: "RESOURCE_MUL",      #a == resource_id, d == factor

                #these are only used in technology trees, 103 even requires one
                101: "TECHCOST_MODIFY",  #a == research_id, b == resource_id, if c == 0: d==absval else: d == relval
                102: "TECH_DISABLE",     #d == research_id
                103: "TECH_TIME_MODIFY", #a == research_id, if c == 0: d==absval else d==relval

                #attribute_id:
                #0: hit points
                #1: line of sight
                #2: garrison capacity
                #3: unit size radius 0
                #4: unit size radius 1
                #5: movement speed
                #6: garrison recovery rate
                #7: unknown
                #8: armor                           #val = val + (256 * armor_id)
                #9: attack                          #val = val + (256 * attack_id)
                #10: attack reloading time
                #11: accuracy percent
                #12: range
                #13: working rate
                #14: resource carriage
                #15: unknown
                #16: new projectile unit
                #17: upgrade graphic
                #18: unknown?
                #19: intelligent projectile aim 1=on, 0=off
                #20: minimum range
                #21: population support
                #22: blast radius (area damage)
                #23: search radius
                #80: boarding energy reload speed
                #100: resource cost
                #101: creation time
                #102: number of garrison arrows
                #103: food cost
                #104: wood cost
                #105: stone cost
                #106: gold cost
                #107: max duplicated missiles
                #108: healing rate
            },
        )),
        (dataformat.READ, "unit",      "int16_t"),      # == a
        (dataformat.READ, "unit_class_id",  "int16_t"), # == b
        (dataformat.READ, "attribute_id", "int16_t"),   # == c
        (dataformat.READ, "amount",    "float"),        # == d
    )


class Tech(dataformat.Exportable):  #also called techage in some other tools
    name_struct        = "tech"
    name_struct_file   = "tech"
    struct_description = "a technology definition."

    data_format = (
        (dataformat.READ, "name", "char[31]"),                  #always CHUN4 (change unit 4-arg)
        (dataformat.READ, "effect_count", "uint16_t"),
        (dataformat.READ, "effects", dataformat.SubdataMember(
            ref_type=Effect,
            length="effect_count",
        )),
    )


class AgeTechTree(dataformat.Exportable):
    name_struct        = "age_tech_tree"
    name_struct_file   = "tech"
    struct_description = "items available when this age was reached."

    data_format = (
        (dataformat.READ_UNKNOWN, None, "int32_t"),
        (dataformat.READ, "id", "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ, "building_count", "int8_t"),
        (dataformat.READ, "buildings", "int32_t[building_count]"),
        (dataformat.READ, "unit_count", "int8_t"),
        (dataformat.READ, "units", "int32_t[unit_count]"),
        (dataformat.READ, "research_count", "int8_t"),
        (dataformat.READ, "researches", "int32_t[research_count]"),
        (dataformat.READ_UNKNOWN, None, "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t"),
        (dataformat.READ, "zeroes", dataformat.ZeroMember("int16_t", length=49)),
    )


class BuildingConnection(dataformat.Exportable):
    name_struct        = "building_connection"
    name_struct_file   = "tech"
    struct_description = "new available buildings/units/researches when this building was created."

    data_format = (
        (dataformat.READ_EXPORT, "id", "int32_t"),         #unit id of the current building
        (dataformat.READ, "prerequisite_count", "int8_t"), #always 2, as we got 2 of them hardcoded below (unit_or_research, mode)
        (dataformat.READ_EXPORT, "building_count", "int8_t"),
        (dataformat.READ, "buildings", "int32_t[building_count]"),  #new buildings available when this building was created
        (dataformat.READ_EXPORT, "unit_count", "int8_t"),
        (dataformat.READ, "units", "int32_t[unit_count]"),          #new units
        (dataformat.READ_EXPORT, "research_count", "int8_t"),
        (dataformat.READ, "researches", "int32_t[research_count]"), #new researches
        (dataformat.READ_EXPORT, "age", "int32_t"),          #minimum age, in which this building is available
        (dataformat.READ, "unit_or_research0", "int32_t"),   #this building depends on research_id or unit_id, which is set in mode0
        (dataformat.READ, "unit_or_research1", "int32_t"),   #dito, set in mode1
        (dataformat.READ_UNKNOWN, None, "int32_t[8]"),
        (dataformat.READ, "mode0", dataformat.EnumLookupMember(  #mode for unit_or_research0
            raw_type = "int32_t",
            type_name = "building_connection_mode",
            lookup_dict = {
                0: "NOTHING",
                1: "BUILDING",
                2: "UNIT",
                3: "RESEARCH",
            }
        )),
        (dataformat.READ, "mode1", "int32_t"), #TODO, xref to the enum above
        (dataformat.READ_UNKNOWN, None, "int32_t[8]"),
        (dataformat.READ_UNKNOWN, None, "int8_t[11]"),
        (dataformat.READ_EXPORT, "connections", "int32_t"),     #5: >=1 connections, 6: no connections
        (dataformat.READ, "enabled_by_research_id", "int32_t"), #current building is unlocked by this research id, -1=no unlock needed
    )


class UnitConnection(dataformat.Exportable):
    name_struct        = "unit_connection"
    name_struct_file   = "tech"
    struct_description = "unit updates to apply when activating the technology."

    data_format = (
        (dataformat.READ, "id", "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ, "upper_building", "int32_t"),        #building, where this unit is created
        (dataformat.READ, "required_researches", "int32_t"),
        (dataformat.READ, "age", "int32_t"),
        (dataformat.READ, "unit_or_research0", "int32_t"),
        (dataformat.READ, "unit_or_research1", "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t[8]"),
        (dataformat.READ, "mode0", "int32_t"),
        (dataformat.READ, "mode1", "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t[7]"),
        (dataformat.READ, "vertical_lines", "int32_t"),
        (dataformat.READ, "unit_count", "int8_t"),
        (dataformat.READ, "units", "int32_t[unit_count]"),
        (dataformat.READ, "location_in_age", "int32_t"),    #0=hidden, 1=first, 2=second
        (dataformat.READ, "required_research", "int32_t"),  #min amount of researches to be discovered for this unit to be available
        (dataformat.READ, "line_mode", "int32_t"),          #0=independent/new in its line, 3=depends on a previous research in its line
        (dataformat.READ, "enabling_research", "int32_t"),
    )


class ResearchConnection(dataformat.Exportable):
    name_struct        = "research_connection"
    name_struct_file   = "tech"
    struct_description = "research updates to apply when activating the technology."

    data_format = (
        (dataformat.READ, "id", "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ, "upper_building", "int32_t"),
        (dataformat.READ, "building_count", "int8_t"),
        (dataformat.READ, "buildings", "int32_t[building_count]"),
        (dataformat.READ, "unit_count", "int8_t"),
        (dataformat.READ, "units", "int32_t[unit_count]"),
        (dataformat.READ, "research_count", "int8_t"),
        (dataformat.READ, "researches", "int32_t[research_count]"),
        (dataformat.READ, "required_research", "int32_t"),
        (dataformat.READ, "age", "int32_t"),
        (dataformat.READ, "upper_research", "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t[9]"),
        (dataformat.READ, "line_mode", "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t[8]"),
        (dataformat.READ, "vertical_line", "int32_t"),
        (dataformat.READ, "location_in_age", "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t"),
    )

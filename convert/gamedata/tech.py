import dataformat
from struct import Struct, unpack_from
from util import dbg, zstr, offset_info

from .empiresdat import endianness


class Effect(dataformat.Exportable):
    name_struct        = "tech_effect"
    name_struct_file   = "tech"
    struct_description = "applied effect for a research technology."

    data_format = (
        (dataformat.READ, "type_id",   "int8_t"),
        (dataformat.READ, "unit",      "int16_t"),
        (dataformat.READ, "class_id",  "int16_t"),
        (dataformat.READ, "attribute", "int16_t"),
        (dataformat.READ, "amount",    "float"),
    )


class Tech(dataformat.Exportable):
    name_struct        = "tech"
    name_struct_file   = "tech"
    struct_description = "a technology definition."

    data_format = (
        (dataformat.READ, "name", "char[31]"),
        (dataformat.READ, "effect_count", "uint16_t"),
        (dataformat.READ, "effects", dataformat.SubdataMember(
            ref_type=Effect,
            length="effect_count",
        )),
    )


class AgeTechTree(dataformat.Exportable):
    name_struct        = "age_tech_tree"
    name_struct_file   = "tech"
    struct_description = "technology tree specification."

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
    struct_description = "building updates to apply when activating the technology."

    data_format = (
        (dataformat.READ, "id", "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ, "building_count", "int8_t"),
        (dataformat.READ, "buildings", "int32_t[building_count]"),
        (dataformat.READ, "unit_count", "int8_t"),
        (dataformat.READ, "units", "int32_t[unit_count]"),
        (dataformat.READ, "research_count", "int8_t"),
        (dataformat.READ, "researches", "int32_t[research_count]"),
        (dataformat.READ, "age", "int32_t"),
        (dataformat.READ, "unit_or_research0", "int32_t"),
        (dataformat.READ, "unit_or_research1", "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t[8]"),
        (dataformat.READ, "mode0", "int32_t"),
        (dataformat.READ, "mode1", "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int32_t[8]"),
        (dataformat.READ_UNKNOWN, None, "int8_t[11]"),
        (dataformat.READ, "connection", "int32_t"),
        (dataformat.READ, "enabling_research", "int32_t"),
    )


class UnitConnection(dataformat.Exportable):
    name_struct        = "unit_connection"
    name_struct_file   = "tech"
    struct_description = "unit updates to apply when activating the technology."

    data_format = (
        (dataformat.READ, "id", "int32_t"),
        (dataformat.READ_UNKNOWN, None, "int8_t"),
        (dataformat.READ, "upper_building", "int32_t"),
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
        (dataformat.READ, "location_in_age", "int32_t"),
        (dataformat.READ, "required_research", "int32_t"),
        (dataformat.READ, "line_mode", "int32_t"),
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

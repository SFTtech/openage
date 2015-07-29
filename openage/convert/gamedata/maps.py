# Copyright 2015-2015 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from ..dataformat.exportable import Exportable
from ..dataformat.members import SubdataMember
from ..dataformat.member_access import READ, READ_UNKNOWN


class MapHeader(Exportable):
    name_struct_file   = "randommap"
    name_struct        = "map_header"
    struct_description = "random map information header"

    data_format = (
        (READ, "script_number", "int32_t"),
        (READ, "border_south_west", "int32_t"),
        (READ, "border_north_west", "int32_t"),
        (READ, "border_north_east", "int32_t"),
        (READ, "border_south_east", "int32_t"),
        (READ, "border_usage", "int32_t"),
        (READ, "water_shape", "int32_t"),
        (READ, "non_base_terrain", "int32_t"),
        (READ, "base_zone_coverage", "int32_t"),
        (READ_UNKNOWN, None, "int32_t"),
        (READ, "base_zone_count", "uint32_t"),
        (READ, "base_zone_ptr", "int32_t"),
        (READ, "map_terrain_count", "uint32_t"),
        (READ, "map_terrain_ptr", "int32_t"),
        (READ, "map_unit_count", "uint32_t"),
        (READ, "map_unit_ptr", "int32_t"),
        (READ_UNKNOWN, None, "uint32_t"),
        (READ_UNKNOWN, None, "int32_t"),
    )


class BaseZone(Exportable):
    name_struct_file   = "randommap"
    name_struct        = "map"
    struct_description = "random map information data"

    data_format = (
        (READ_UNKNOWN, None, "int32_t"),
        (READ, "base_terrain", "int32_t"),
        (READ, "spacing_between_players", "int32_t"),
        (READ_UNKNOWN, None, "int32_t"),
        (READ_UNKNOWN, None, "int8_t[4]"),
        (READ_UNKNOWN, None, "int32_t"),
        (READ_UNKNOWN, None, "int32_t"),
        (READ_UNKNOWN, None, "int8_t[4]"),
        (READ, "start_area_radius", "int32_t"),
        (READ_UNKNOWN, None, "int32_t"),
        (READ_UNKNOWN, None, "int32_t"),
    )


class MapTerrain(Exportable):
    name_struct_file   = "randommap"
    name_struct        = "map_terrain"
    struct_description = "random map terrain information data"

    data_format = (
        (READ, "proportion", "int32_t"),
        (READ, "terrain", "int32_t"),
        (READ, "number_of_clumps", "int32_t"),
        (READ, "spacing_to_other_terrains", "int32_t"),
        (READ, "placement_zone", "int32_t"),
        (READ_UNKNOWN, None, "int32_t"),
    )


class MapUnit(Exportable):
    name_struct_file   = "randommap"
    name_struct        = "map_unit"
    struct_description = "random map unit information data"

    data_format = (
        (READ, "unit", "int32_t"),
        (READ, "host_terrain", "int32_t"),
        (READ_UNKNOWN, None, "int8_t[4]"),
        (READ, "objects_per_group", "int32_t"),
        (READ, "fluctuation", "int32_t"),
        (READ, "groups_per_player", "int32_t"),
        (READ, "group_radius", "int32_t"),
        (READ, "own_at_start", "int32_t"),
        (READ, "set_place_for_all_players", "int32_t"),
        (READ, "min_distance_to_players", "int32_t"),
        (READ, "max_distance_to_players", "int32_t"),
    )


class MapUnknown(Exportable):
    name_struct_file   = "randommap"
    name_struct        = "map_unknown"
    struct_description = "random map infos about magic unknown things"

    data_format = (
        (READ_UNKNOWN, None, "int32_t[6]"),
    )


class Map(Exportable):
    name_struct_file   = "randommap"
    name_struct        = "map"
    struct_description = "random map information data"

    data_format = (
        (READ, "border_south_west", "int32_t"),
        (READ, "border_north_west", "int32_t"),
        (READ, "border_north_east", "int32_t"),
        (READ, "border_south_east", "int32_t"),
        (READ, "border_usage", "int32_t"),
        (READ, "water_shape", "int32_t"),
        (READ, "non_base_terrain", "int32_t"),
        (READ, "base_zone_coverage", "int32_t"),
        (READ_UNKNOWN, None, "int32_t"),

        (READ, "base_zone_count", "uint32_t"),
        (READ, "base_zone_ptr", "int32_t"),
        (READ, "base_zones", SubdataMember(
            ref_type=BaseZone,
            length="base_zone_count",
        )),

        (READ, "map_terrain_count", "uint32_t"),
        (READ, "map_terrain_ptr", "int32_t"),
        (READ, "map_terrains", SubdataMember(
            ref_type=MapTerrain,
            length="map_terrain_count",
        )),

        (READ, "map_unit_count", "uint32_t"),
        (READ, "map_unit_ptr", "int32_t"),
        (READ, "map_units", SubdataMember(
            ref_type=MapUnit,
            length="map_unit_count",
        )),

        (READ, "map_unknown_count", "uint32_t"),
        (READ, "map_unknown_ptr", "int32_t"),
        (READ, "map_unknowns", SubdataMember(
            ref_type=MapUnknown,
            length="map_unknown_count",
        )),
    )

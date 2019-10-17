# Copyright 2015-2017 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from openage.convert.dataformat.genie_structure import GenieStructure
from openage.convert.dataformat.read_members import SubdataMember
from ..dataformat.member_access import READ


class MapInfo(GenieStructure):
    name_struct_file   = "randommap"
    name_struct        = "map_header"
    struct_description = "random map information header"

    data_format = [
        (READ, "map_id", "int32_t"),
        (READ, "border_south_west", "int32_t"),
        (READ, "border_north_west", "int32_t"),
        (READ, "border_north_east", "int32_t"),
        (READ, "border_south_east", "int32_t"),
        (READ, "border_usage", "int32_t"),
        (READ, "water_shape", "int32_t"),
        (READ, "base_terrain", "int32_t"),
        (READ, "land_coverage", "int32_t"),
        (READ, "unused_id", "int32_t"),
        (READ, "base_zone_count", "uint32_t"),
        (READ, "base_zone_ptr", "int32_t"),
        (READ, "map_terrain_count", "uint32_t"),
        (READ, "map_terrain_ptr", "int32_t"),
        (READ, "map_unit_count", "uint32_t"),
        (READ, "map_unit_ptr", "int32_t"),
        (READ, "map_elevation_count", "uint32_t"),
        (READ, "map_elevation_ptr", "int32_t"),
    ]


class MapLand(GenieStructure):
    name_struct_file   = "randommap"
    name_struct        = "map"
    struct_description = "random map information data"

    data_format = [
        (READ, "land_id", "int32_t"),
        (READ, "terrain", "int32_t"),
        (READ, "land_spacing", "int32_t"),
        (READ, "base_size", "int32_t"),
        (READ, "zone", "int8_t"),
        (READ, "placement_type", "int8_t"),
        (READ, "padding1", "int16_t"),
        (READ, "base_x", "int32_t"),
        (READ, "base_y", "int32_t"),
        (READ, "land_proportion", "int8_t"),
        (READ, "by_player_flag", "int8_t"),
        (READ, "padding2", "int16_t"),
        (READ, "start_area_radius", "int32_t"),
        (READ, "terrain_edge_fade", "int32_t"),
        (READ, "clumpiness", "int32_t"),
    ]


class MapTerrain(GenieStructure):
    name_struct_file   = "randommap"
    name_struct        = "map_terrain"
    struct_description = "random map terrain information data"

    data_format = [
        (READ, "proportion", "int32_t"),
        (READ, "terrain", "int32_t"),
        (READ, "number_of_clumps", "int32_t"),
        (READ, "edge_spacing", "int32_t"),
        (READ, "placement_zone", "int32_t"),
        (READ, "clumpiness", "int32_t"),
    ]


class MapUnit(GenieStructure):
    name_struct_file   = "randommap"
    name_struct        = "map_unit"
    struct_description = "random map unit information data"

    data_format = [
        (READ, "unit", "int32_t"),
        (READ, "host_terrain", "int32_t"),
        (READ, "group_placing", "int8_t"),
        (READ, "scale_flag", "int8_t"),
        (READ, "padding1", "int16_t"),
        (READ, "objects_per_group", "int32_t"),
        (READ, "fluctuation", "int32_t"),
        (READ, "groups_per_player", "int32_t"),
        (READ, "group_radius", "int32_t"),
        (READ, "own_at_start", "int32_t"),
        (READ, "set_place_for_all_players", "int32_t"),
        (READ, "min_distance_to_players", "int32_t"),
        (READ, "max_distance_to_players", "int32_t"),
    ]


class MapElevation(GenieStructure):
    name_struct_file   = "randommap"
    name_struct        = "map_elevation"
    struct_description = "random map elevation data"

    data_format = [
        (READ, "proportion", "int32_t"),
        (READ, "terrain", "int32_t"),
        (READ, "clump_count", "int32_t"),
        (READ, "base_terrain", "int32_t"),
        (READ, "base_elevation", "int32_t"),
        (READ, "tile_spacing", "int32_t"),
    ]


class Map(GenieStructure):
    name_struct_file   = "randommap"
    name_struct        = "map"
    struct_description = "random map information data"

    data_format = [
        (READ, "border_south_west", "int32_t"),
        (READ, "border_north_west", "int32_t"),
        (READ, "border_north_east", "int32_t"),
        (READ, "border_south_east", "int32_t"),
        (READ, "border_usage", "int32_t"),
        (READ, "water_shape", "int32_t"),
        (READ, "base_terrain", "int32_t"),
        (READ, "land_coverage", "int32_t"),
        (READ, "unused_id", "int32_t"),

        (READ, "base_zone_count", "uint32_t"),
        (READ, "base_zone_ptr", "int32_t"),
        (READ, "base_zones", SubdataMember(
            ref_type=MapLand,
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

        (READ, "map_elevation_count", "uint32_t"),
        (READ, "map_elevation_ptr", "int32_t"),
        (READ, "map_elevations", SubdataMember(
            ref_type=MapElevation,
            length="map_elevation_count",
        )),
    ]

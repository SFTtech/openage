# Copyright 2015-2017 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R

from ..dataformat.exportable import Exportable
from ..dataformat.members import SubdataMember
from ..dataformat.member_access import READ


class MapInfo(Exportable):
    name_struct_file   = "randommap"
    name_struct        = "map_header"
    struct_description = "random map information header"

    data_format = []
    data_format.append((READ, "map_id", "int32_t"))
    data_format.append((READ, "border_south_west", "int32_t"))
    data_format.append((READ, "border_north_west", "int32_t"))
    data_format.append((READ, "border_north_east", "int32_t"))
    data_format.append((READ, "border_south_east", "int32_t"))
    data_format.append((READ, "border_usage", "int32_t"))
    data_format.append((READ, "water_shape", "int32_t"))
    data_format.append((READ, "base_terrain", "int32_t"))
    data_format.append((READ, "land_coverage", "int32_t"))
    data_format.append((READ, "unused_id", "int32_t"))
    data_format.append((READ, "base_zone_count", "uint32_t"))
    data_format.append((READ, "base_zone_ptr", "int32_t"))
    data_format.append((READ, "map_terrain_count", "uint32_t"))
    data_format.append((READ, "map_terrain_ptr", "int32_t"))
    data_format.append((READ, "map_unit_count", "uint32_t"))
    data_format.append((READ, "map_unit_ptr", "int32_t"))
    data_format.append((READ, "map_elevation_count", "uint32_t"))
    data_format.append((READ, "map_elevation_ptr", "int32_t"))


class MapLand(Exportable):
    name_struct_file   = "randommap"
    name_struct        = "map"
    struct_description = "random map information data"

    data_format = []
    data_format.append((READ, "land_id", "int32_t"))
    data_format.append((READ, "terrain", "int32_t"))
    data_format.append((READ, "land_spacing", "int32_t"))
    data_format.append((READ, "base_size", "int32_t"))
    data_format.append((READ, "zone", "int8_t"))
    data_format.append((READ, "placement_type", "int8_t"))
    data_format.append((READ, "padding1", "int16_t"))
    data_format.append((READ, "base_x", "int32_t"))
    data_format.append((READ, "base_y", "int32_t"))
    data_format.append((READ, "land_proportion", "int8_t"))
    data_format.append((READ, "by_player_flag", "int8_t"))
    data_format.append((READ, "padding2", "int16_t"))
    data_format.append((READ, "start_area_radius", "int32_t"))
    data_format.append((READ, "terrain_edge_fade", "int32_t"))
    data_format.append((READ, "clumpiness", "int32_t"))


class MapTerrain(Exportable):
    name_struct_file   = "randommap"
    name_struct        = "map_terrain"
    struct_description = "random map terrain information data"

    data_format = []
    data_format.append((READ, "proportion", "int32_t"))
    data_format.append((READ, "terrain", "int32_t"))
    data_format.append((READ, "number_of_clumps", "int32_t"))
    data_format.append((READ, "edge_spacing", "int32_t"))
    data_format.append((READ, "placement_zone", "int32_t"))
    data_format.append((READ, "clumpiness", "int32_t"))


class MapUnit(Exportable):
    name_struct_file   = "randommap"
    name_struct        = "map_unit"
    struct_description = "random map unit information data"

    data_format = []
    data_format.append((READ, "unit", "int32_t"))
    data_format.append((READ, "host_terrain", "int32_t"))
    data_format.append((READ, "group_placing", "int8_t"))
    data_format.append((READ, "scale_flag", "int8_t"))
    data_format.append((READ, "padding1", "int16_t"))
    data_format.append((READ, "objects_per_group", "int32_t"))
    data_format.append((READ, "fluctuation", "int32_t"))
    data_format.append((READ, "groups_per_player", "int32_t"))
    data_format.append((READ, "group_radius", "int32_t"))
    data_format.append((READ, "own_at_start", "int32_t"))
    data_format.append((READ, "set_place_for_all_players", "int32_t"))
    data_format.append((READ, "min_distance_to_players", "int32_t"))
    data_format.append((READ, "max_distance_to_players", "int32_t"))


class MapElevation(Exportable):
    name_struct_file   = "randommap"
    name_struct        = "map_elevation"
    struct_description = "random map infos about magic unknown things"

    data_format = []
    data_format.append((READ, "proportion", "int32_t"))
    data_format.append((READ, "terrain", "int32_t"))
    data_format.append((READ, "clump_count", "int32_t"))
    data_format.append((READ, "base_terrain", "int32_t"))
    data_format.append((READ, "base_elevation", "int32_t"))
    data_format.append((READ, "tile_spacing", "int32_t"))


class Map(Exportable):
    name_struct_file   = "randommap"
    name_struct        = "map"
    struct_description = "random map information data"

    data_format = []
    data_format.append((READ, "border_south_west", "int32_t"))
    data_format.append((READ, "border_north_west", "int32_t"))
    data_format.append((READ, "border_north_east", "int32_t"))
    data_format.append((READ, "border_south_east", "int32_t"))
    data_format.append((READ, "border_usage", "int32_t"))
    data_format.append((READ, "water_shape", "int32_t"))
    data_format.append((READ, "base_terrain", "int32_t"))
    data_format.append((READ, "land_coverage", "int32_t"))
    data_format.append((READ, "unused_id", "int32_t"))

    data_format.append((READ, "base_zone_count", "uint32_t"))
    data_format.append((READ, "base_zone_ptr", "int32_t"))
    data_format.append((READ, "base_zones", SubdataMember(
            ref_type=MapLand,
            length="base_zone_count",
        )))

    data_format.append((READ, "map_terrain_count", "uint32_t"))
    data_format.append((READ, "map_terrain_ptr", "int32_t"))
    data_format.append((READ, "map_terrains", SubdataMember(
            ref_type=MapTerrain,
            length="map_terrain_count",
        )))

    data_format.append((READ, "map_unit_count", "uint32_t"))
    data_format.append((READ, "map_unit_ptr", "int32_t"))
    data_format.append((READ, "map_units", SubdataMember(
            ref_type=MapUnit,
            length="map_unit_count",
        )))

    data_format.append((READ, "map_elevation_count", "uint32_t"))
    data_format.append((READ, "map_elevation_ptr", "int32_t"))
    data_format.append((READ, "map_elevations", SubdataMember(
            ref_type=MapElevation,
            length="map_elevation_count",
        )))

# Copyright 2015-2023 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R
from __future__ import annotations
import typing

from functools import cache

from ...genie_structure import GenieStructure
from ....read.member_access import READ, SKIP
from ....read.read_members import SubdataMember
from ....read.value_members import StorageType

if typing.TYPE_CHECKING:
    from openage.convert.value_object.init.game_version import GameVersion
    from openage.convert.value_object.read.member_access import MemberAccess
    from openage.convert.value_object.read.read_members import ReadMember


class MapInfo(GenieStructure):

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ, "map_id", StorageType.ID_MEMBER, "int32_t"),
            (READ, "border_south_west", StorageType.INT_MEMBER, "int32_t"),
            (READ, "border_north_west", StorageType.INT_MEMBER, "int32_t"),
            (READ, "border_north_east", StorageType.INT_MEMBER, "int32_t"),
            (READ, "border_south_east", StorageType.INT_MEMBER, "int32_t"),
            (READ, "border_usage", StorageType.INT_MEMBER, "int32_t"),
            (READ, "water_shape", StorageType.INT_MEMBER, "int32_t"),
            (READ, "base_terrain", StorageType.INT_MEMBER, "int32_t"),
            (READ, "land_coverage", StorageType.INT_MEMBER, "int32_t"),
            (SKIP, "unused_id", StorageType.ID_MEMBER, "int32_t"),
            (READ, "base_zone_count", StorageType.INT_MEMBER, "uint32_t"),
            (READ, "base_zone_ptr", StorageType.ID_MEMBER, "int32_t"),
            (READ, "map_terrain_count", StorageType.INT_MEMBER, "uint32_t"),
            (READ, "map_terrain_ptr", StorageType.ID_MEMBER, "int32_t"),
            (READ, "map_unit_count", StorageType.INT_MEMBER, "uint32_t"),
            (READ, "map_unit_ptr", StorageType.ID_MEMBER, "int32_t"),
            (READ, "map_elevation_count", StorageType.INT_MEMBER, "uint32_t"),
            (READ, "map_elevation_ptr", StorageType.ID_MEMBER, "int32_t"),
        ]

        return data_format


class MapLand(GenieStructure):

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ, "land_id", StorageType.ID_MEMBER, "int32_t"),
            (READ, "terrain", StorageType.ID_MEMBER, "int32_t"),
            (READ, "land_spacing", StorageType.INT_MEMBER, "int32_t"),
            (READ, "base_size", StorageType.INT_MEMBER, "int32_t"),
            (READ, "zone", StorageType.INT_MEMBER, "int8_t"),
            (READ, "placement_type", StorageType.ID_MEMBER, "int8_t"),
            (SKIP, "padding1", StorageType.INT_MEMBER, "int16_t"),
            (READ, "base_x", StorageType.INT_MEMBER, "int32_t"),
            (READ, "base_y", StorageType.INT_MEMBER, "int32_t"),
            (READ, "land_proportion", StorageType.INT_MEMBER, "int8_t"),
            (READ, "by_player_flag", StorageType.ID_MEMBER, "int8_t"),
            (SKIP, "padding2", StorageType.INT_MEMBER, "int16_t"),
            (READ, "start_area_radius", StorageType.INT_MEMBER, "int32_t"),
            (READ, "terrain_edge_fade", StorageType.INT_MEMBER, "int32_t"),
            (READ, "clumpiness", StorageType.INT_MEMBER, "int32_t"),
        ]

        return data_format


class MapTerrain(GenieStructure):

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ, "proportion", StorageType.INT_MEMBER, "int32_t"),
            (READ, "terrain_id", StorageType.ID_MEMBER, "int32_t"),
            (READ, "number_of_clumps", StorageType.INT_MEMBER, "int32_t"),
            (READ, "edge_spacing", StorageType.INT_MEMBER, "int32_t"),
            (READ, "placement_zone", StorageType.INT_MEMBER, "int32_t"),
            (READ, "clumpiness", StorageType.INT_MEMBER, "int32_t"),
        ]

        return data_format


class MapUnit(GenieStructure):

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ, "unit_id", StorageType.ID_MEMBER, "int32_t"),
            (READ, "host_terrain", StorageType.ID_MEMBER, "int32_t"),   # -1 = land; 1 = water
            (READ, "group_placing", StorageType.ID_MEMBER, "int8_t"),   # 0 =
            (READ, "scale_flag", StorageType.BOOLEAN_MEMBER, "int8_t"),
            (SKIP, "padding1", StorageType.INT_MEMBER, "int16_t"),
            (READ, "objects_per_group", StorageType.INT_MEMBER, "int32_t"),
            (READ, "fluctuation", StorageType.INT_MEMBER, "int32_t"),
            (READ, "groups_per_player", StorageType.INT_MEMBER, "int32_t"),
            (READ, "group_radius", StorageType.INT_MEMBER, "int32_t"),
            (READ, "own_at_start", StorageType.INT_MEMBER, "int32_t"),  # -1 = player unit; 0 = else
            (READ, "set_place_for_all_players", StorageType.INT_MEMBER, "int32_t"),
            (READ, "min_distance_to_players", StorageType.INT_MEMBER, "int32_t"),
            (READ, "max_distance_to_players", StorageType.INT_MEMBER, "int32_t"),
        ]

        return data_format


class MapElevation(GenieStructure):

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ, "proportion", StorageType.INT_MEMBER, "int32_t"),
            (READ, "terrain", StorageType.INT_MEMBER, "int32_t"),
            (READ, "clump_count", StorageType.INT_MEMBER, "int32_t"),
            (READ, "base_terrain", StorageType.ID_MEMBER, "int32_t"),
            (READ, "base_elevation", StorageType.INT_MEMBER, "int32_t"),
            (READ, "tile_spacing", StorageType.INT_MEMBER, "int32_t"),
        ]

        return data_format


class Map(GenieStructure):

    @classmethod
    @cache
    def get_data_format_members(
        cls,
        game_version: GameVersion
    ) -> list[tuple[MemberAccess, str, StorageType, typing.Union[str, ReadMember]]]:
        """
        Return the members in this struct.
        """
        data_format = [
            (READ, "border_south_west", StorageType.INT_MEMBER, "int32_t"),
            (READ, "border_north_west", StorageType.INT_MEMBER, "int32_t"),
            (READ, "border_north_east", StorageType.INT_MEMBER, "int32_t"),
            (READ, "border_south_east", StorageType.INT_MEMBER, "int32_t"),
            (READ, "border_usage", StorageType.INT_MEMBER, "int32_t"),
            (READ, "water_shape", StorageType.INT_MEMBER, "int32_t"),
            (READ, "base_terrain", StorageType.INT_MEMBER, "int32_t"),
            (READ, "land_coverage", StorageType.INT_MEMBER, "int32_t"),
            (SKIP, "unused_id", StorageType.ID_MEMBER, "int32_t"),

            (READ, "base_zone_count", StorageType.INT_MEMBER, "uint32_t"),
            (READ, "base_zone_ptr", StorageType.ID_MEMBER, "int32_t"),
            (READ, "base_zones", StorageType.ARRAY_CONTAINER, SubdataMember(
                ref_type=MapLand,
                length="base_zone_count",
            )),

            (READ, "map_terrain_count", StorageType.INT_MEMBER, "uint32_t"),
            (READ, "map_terrain_ptr", StorageType.ID_MEMBER, "int32_t"),
            (READ, "map_terrains", StorageType.ARRAY_CONTAINER, SubdataMember(
                ref_type=MapTerrain,
                length="map_terrain_count",
            )),

            (READ, "map_unit_count", StorageType.INT_MEMBER, "uint32_t"),
            (READ, "map_unit_ptr", StorageType.ID_MEMBER, "int32_t"),
            (READ, "map_units", StorageType.ARRAY_CONTAINER, SubdataMember(
                ref_type=MapUnit,
                length="map_unit_count",
            )),

            (READ, "map_elevation_count", StorageType.INT_MEMBER, "uint32_t"),
            (READ, "map_elevation_ptr", StorageType.ID_MEMBER, "int32_t"),
            (READ, "map_elevations", StorageType.ARRAY_CONTAINER, SubdataMember(
                ref_type=MapElevation,
                length="map_elevation_count",
            )),
        ]

        return data_format

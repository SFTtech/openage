# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create terrain types for AoC.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject, ConverterObjectGroup
from .....service.conversion import internal_name_lookups

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def generate_terrain_types(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate TerrainType objects.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    terrain_type_lookup_dict = internal_name_lookups.get_terrain_type_lookups(
        full_data_set.game_version)

    type_parent = "engine.util.terrain_type.TerrainType"
    types_location = "data/util/terrain_type/"

    terrain_type_lookups = terrain_type_lookup_dict.values()

    for terrain_type in terrain_type_lookups:
        type_name = terrain_type[2]
        type_ref_in_modpack = f"util.terrain_type.types.{type_name}"
        type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                           type_name, api_objects,
                                           types_location)
        type_raw_api_object.set_filename("types")
        type_raw_api_object.add_raw_parent(type_parent)

        pregen_converter_group.add_raw_api_object(type_raw_api_object)
        pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

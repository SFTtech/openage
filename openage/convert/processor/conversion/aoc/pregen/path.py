# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create path types for AoC.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject, ConverterObjectGroup

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer

PATH_TYPE_PARENT = "engine.util.path_type.PathType"
PATH_TYPES_LOCATION = "data/util/path_type/"


def generate_path_types(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate PathType objects.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    # =======================================================================
    # Land
    # =======================================================================
    path_type_ref_in_modpack = "util.path.types.Land"
    path_type_raw_api_object = RawAPIObject(path_type_ref_in_modpack,
                                            "Land",
                                            api_objects,
                                            PATH_TYPES_LOCATION)
    path_type_raw_api_object.set_filename("types")
    path_type_raw_api_object.add_raw_parent(PATH_TYPE_PARENT)

    pregen_converter_group.add_raw_api_object(path_type_raw_api_object)
    pregen_nyan_objects.update({path_type_ref_in_modpack: path_type_raw_api_object})

    # =======================================================================
    # Water
    # =======================================================================
    path_type_ref_in_modpack = "util.path.types.Water"
    path_type_raw_api_object = RawAPIObject(path_type_ref_in_modpack,
                                            "Water",
                                            api_objects,
                                            PATH_TYPES_LOCATION)
    path_type_raw_api_object.set_filename("types")
    path_type_raw_api_object.add_raw_parent(PATH_TYPE_PARENT)

    pregen_converter_group.add_raw_api_object(path_type_raw_api_object)
    pregen_nyan_objects.update({path_type_ref_in_modpack: path_type_raw_api_object})

    # =======================================================================
    # Air
    # =======================================================================
    path_type_ref_in_modpack = "util.path.types.Air"
    path_type_raw_api_object = RawAPIObject(path_type_ref_in_modpack,
                                            "Air",
                                            api_objects,
                                            PATH_TYPES_LOCATION)
    path_type_raw_api_object.set_filename("types")
    path_type_raw_api_object.add_raw_parent(PATH_TYPE_PARENT)

    pregen_converter_group.add_raw_api_object(path_type_raw_api_object)
    pregen_nyan_objects.update({path_type_ref_in_modpack: path_type_raw_api_object})

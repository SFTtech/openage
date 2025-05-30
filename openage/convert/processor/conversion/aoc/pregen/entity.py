# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create entity types for AoC.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject, ConverterObjectGroup
from .....service.conversion import internal_name_lookups

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer

TYPE_PARENT = "engine.util.game_entity_type.GameEntityType"
TYPES_LOCATION = "data/util/game_entity_type/"


def generate_entity_types(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate GameEntityType objects.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    _generate_ambient_entity_type(full_data_set, pregen_converter_group)
    _generate_building_entity_type(full_data_set, pregen_converter_group)
    _generate_item_entity_type(full_data_set, pregen_converter_group)
    _generate_unit_entity_type(full_data_set, pregen_converter_group)
    _generate_projectile_entity_type(full_data_set, pregen_converter_group)
    _generate_drop_site_entity_type(full_data_set, pregen_converter_group)
    _generate_class_entity_types(full_data_set, pregen_converter_group)


def _generate_ambient_entity_type(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the Ambient GameEntityType.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    ambient_ref_in_modpack = "util.game_entity_type.types.Ambient"
    ambient_raw_api_object = RawAPIObject(ambient_ref_in_modpack,
                                          "Ambient", api_objects,
                                          TYPES_LOCATION)
    ambient_raw_api_object.set_filename("types")
    ambient_raw_api_object.add_raw_parent(TYPE_PARENT)

    pregen_converter_group.add_raw_api_object(ambient_raw_api_object)
    pregen_nyan_objects.update({ambient_ref_in_modpack: ambient_raw_api_object})


def _generate_building_entity_type(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the Building GameEntityType.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    building_ref_in_modpack = "util.game_entity_type.types.Building"
    building_raw_api_object = RawAPIObject(building_ref_in_modpack,
                                           "Building", api_objects,
                                           TYPES_LOCATION)
    building_raw_api_object.set_filename("types")
    building_raw_api_object.add_raw_parent(TYPE_PARENT)

    pregen_converter_group.add_raw_api_object(building_raw_api_object)
    pregen_nyan_objects.update({building_ref_in_modpack: building_raw_api_object})


def _generate_item_entity_type(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the Item GameEntityType.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    item_ref_in_modpack = "util.game_entity_type.types.Item"
    item_raw_api_object = RawAPIObject(item_ref_in_modpack,
                                       "Item", api_objects,
                                       TYPES_LOCATION)
    item_raw_api_object.set_filename("types")
    item_raw_api_object.add_raw_parent(TYPE_PARENT)

    pregen_converter_group.add_raw_api_object(item_raw_api_object)
    pregen_nyan_objects.update({item_ref_in_modpack: item_raw_api_object})


def _generate_unit_entity_type(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the Unit GameEntityType.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    unit_ref_in_modpack = "util.game_entity_type.types.Unit"
    unit_raw_api_object = RawAPIObject(unit_ref_in_modpack,
                                       "Unit", api_objects,
                                       TYPES_LOCATION)
    unit_raw_api_object.set_filename("types")
    unit_raw_api_object.add_raw_parent(TYPE_PARENT)

    pregen_converter_group.add_raw_api_object(unit_raw_api_object)
    pregen_nyan_objects.update({unit_ref_in_modpack: unit_raw_api_object})


def _generate_projectile_entity_type(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the Projectile GameEntityType.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    projectile_ref_in_modpack = "util.game_entity_type.types.Projectile"
    projectile_raw_api_object = RawAPIObject(projectile_ref_in_modpack,
                                             "Projectile", api_objects,
                                             TYPES_LOCATION)
    projectile_raw_api_object.set_filename("types")
    projectile_raw_api_object.add_raw_parent(TYPE_PARENT)

    pregen_converter_group.add_raw_api_object(projectile_raw_api_object)
    pregen_nyan_objects.update({projectile_ref_in_modpack: projectile_raw_api_object})


def _generate_drop_site_entity_type(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the DropSite GameEntityType.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    drop_site_ref_in_modpack = "util.game_entity_type.types.DropSite"
    drop_site_raw_api_object = RawAPIObject(drop_site_ref_in_modpack,
                                            "DropSite", api_objects,
                                            TYPES_LOCATION)
    drop_site_raw_api_object.set_filename("types")
    drop_site_raw_api_object.add_raw_parent(TYPE_PARENT)

    pregen_converter_group.add_raw_api_object(drop_site_raw_api_object)
    pregen_nyan_objects.update({drop_site_ref_in_modpack: drop_site_raw_api_object})


def _generate_class_entity_types(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate GameEntityType objects generated from class IDs.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    class_lookup_dict = internal_name_lookups.get_class_lookups(full_data_set.game_version)

    converter_groups = []
    converter_groups.extend(full_data_set.unit_lines.values())
    converter_groups.extend(full_data_set.building_lines.values())
    converter_groups.extend(full_data_set.ambient_groups.values())
    converter_groups.extend(full_data_set.variant_groups.values())

    for unit_line in converter_groups:
        unit_class = unit_line.get_class_id()
        class_name = class_lookup_dict[unit_class]
        class_obj_name = f"util.game_entity_type.types.{class_name}"

        new_game_entity_type = RawAPIObject(class_obj_name, class_name,
                                            api_objects,
                                            TYPES_LOCATION)
        new_game_entity_type.set_filename("types")
        new_game_entity_type.add_raw_parent("engine.util.game_entity_type.GameEntityType")
        new_game_entity_type.create_nyan_object()

        pregen_converter_group.add_raw_api_object(new_game_entity_type)
        pregen_nyan_objects.update({class_obj_name: new_game_entity_type})

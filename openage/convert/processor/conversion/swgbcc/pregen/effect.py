# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create effect objects for SWGB.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....entity_object.conversion.swgbcc.genie_unit import SWGBUnitTransformGroup
from .....service.conversion import internal_name_lookups
from ...aoc.pregen.effect import ATTRIBUTE_CHANGE_PARENT, ATTRIBUTE_CHANGE_LOCATION
from ...aoc.pregen import effect as aoc_pregen_effect


if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from .....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def generate_effect_types(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate types for effects and resistances.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    _generate_attribute_change_types(full_data_set, pregen_converter_group)
    aoc_pregen_effect.generate_construct_types(full_data_set, pregen_converter_group)
    aoc_pregen_effect.generate_convert_types(full_data_set, pregen_converter_group)


def _generate_attribute_change_types(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate the attribute change types for effects and resistances from
    the armor class lookups.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(full_data_set.game_version)
    armor_lookup_dict = internal_name_lookups.get_armor_class_lookups(
        full_data_set.game_version)

    # =======================================================================
    # Armor types
    # =======================================================================
    for type_name in armor_lookup_dict.values():
        type_ref_in_modpack = f"util.attribute_change_type.types.{type_name}"
        type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                           type_name, api_objects,
                                           ATTRIBUTE_CHANGE_LOCATION)
        type_raw_api_object.set_filename("types")
        type_raw_api_object.add_raw_parent(ATTRIBUTE_CHANGE_PARENT)

        pregen_converter_group.add_raw_api_object(type_raw_api_object)
        pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

    # =======================================================================
    # Heal
    # =======================================================================
    type_ref_in_modpack = "util.attribute_change_type.types.Heal"
    type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                       "Heal", api_objects,
                                       ATTRIBUTE_CHANGE_LOCATION)
    type_raw_api_object.set_filename("types")
    type_raw_api_object.add_raw_parent(ATTRIBUTE_CHANGE_PARENT)

    pregen_converter_group.add_raw_api_object(type_raw_api_object)
    pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

    # =======================================================================
    # Repair (one for each repairable entity)
    # =======================================================================
    repairable_lines = []
    repairable_lines.extend(full_data_set.building_lines.values())
    for unit_line in full_data_set.unit_lines.values():
        if unit_line.is_repairable():
            repairable_lines.append(unit_line)

    for repairable_line in repairable_lines:
        if isinstance(repairable_line, SWGBUnitTransformGroup):
            game_entity_name = name_lookup_dict[repairable_line.get_transform_unit_id()][0]

        else:
            game_entity_name = name_lookup_dict[repairable_line.get_head_unit_id()][0]

        type_ref_in_modpack = f"util.attribute_change_type.types.{game_entity_name}Repair"
        type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                           f"{game_entity_name}Repair",
                                           api_objects,
                                           ATTRIBUTE_CHANGE_LOCATION)
        type_raw_api_object.set_filename("types")
        type_raw_api_object.add_raw_parent(ATTRIBUTE_CHANGE_PARENT)

        pregen_converter_group.add_raw_api_object(type_raw_api_object)
        pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

    # =======================================================================
    # Construct (two for each constructable entity)
    # =======================================================================
    constructable_lines = []
    constructable_lines.extend(full_data_set.building_lines.values())

    for constructable_line in constructable_lines:
        game_entity_name = name_lookup_dict[constructable_line.get_head_unit_id()][0]

        type_ref_in_modpack = f"util.attribute_change_type.types.{game_entity_name}Construct"
        type_raw_api_object = RawAPIObject(type_ref_in_modpack,
                                           f"{game_entity_name}Construct",
                                           api_objects,
                                           ATTRIBUTE_CHANGE_LOCATION)
        type_raw_api_object.set_filename("types")
        type_raw_api_object.add_raw_parent(ATTRIBUTE_CHANGE_PARENT)

        pregen_converter_group.add_raw_api_object(type_raw_api_object)
        pregen_nyan_objects.update({type_ref_in_modpack: type_raw_api_object})

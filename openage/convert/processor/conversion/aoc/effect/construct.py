# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create effects for constructing buildings.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def get_construct_effects(
    line: GenieGameEntityGroup,
    location_ref: str
) -> list[ForwardRef]:
    """
    Creates effects that are used for construction (unit command: 101)

    :param line: Unit/Building line that gets the ability.
    :param location_ref: Reference to API object the effects are added to.
    :returns: The forward references for the effects.
    """
    dataset = line.data

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    effects = []

    progress_effect_parent = "engine.effect.continuous.time_relative_progress.TimeRelativeProgressChange"
    progress_construct_parent = "engine.effect.continuous.time_relative_progress.type.TimeRelativeProgressIncrease"
    attr_effect_parent = "engine.effect.continuous.time_relative_attribute.TimeRelativeAttributeChange"
    attr_construct_parent = "engine.effect.continuous.time_relative_attribute.type.TimeRelativeAttributeIncrease"

    constructable_lines = []
    constructable_lines.extend(dataset.building_lines.values())

    for constructable_line in constructable_lines:
        game_entity_name = name_lookup_dict[constructable_line.get_head_unit_id()][0]

        # Construction progress
        contruct_progress_name = f"{game_entity_name}ConstructProgressEffect"
        contruct_progress_ref = f"{location_ref}.{contruct_progress_name}"
        contruct_progress_raw_api_object = RawAPIObject(contruct_progress_ref,
                                                        contruct_progress_name,
                                                        dataset.nyan_api_objects)
        contruct_progress_raw_api_object.add_raw_parent(progress_construct_parent)
        contruct_progress_location = ForwardRef(line, location_ref)
        contruct_progress_raw_api_object.set_location(contruct_progress_location)

        # Type
        type_ref = f"util.construct_type.types.{game_entity_name}Construct"
        change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
        contruct_progress_raw_api_object.add_raw_member("type",
                                                        change_type,
                                                        progress_effect_parent)

        # Total change time
        change_time = constructable_line.get_head_unit()["creation_time"].value
        contruct_progress_raw_api_object.add_raw_member("total_change_time",
                                                        change_time,
                                                        progress_effect_parent)

        line.add_raw_api_object(contruct_progress_raw_api_object)
        contruct_progress_forward_ref = ForwardRef(line, contruct_progress_ref)
        effects.append(contruct_progress_forward_ref)

        # HP increase during construction
        contruct_hp_name = f"{game_entity_name}ConstructHPEffect"
        contruct_hp_ref = f"{location_ref}.{contruct_hp_name}"
        contruct_hp_raw_api_object = RawAPIObject(contruct_hp_ref,
                                                  contruct_hp_name,
                                                  dataset.nyan_api_objects)
        contruct_hp_raw_api_object.add_raw_parent(attr_construct_parent)
        contruct_hp_location = ForwardRef(line, location_ref)
        contruct_hp_raw_api_object.set_location(contruct_hp_location)

        # Type
        type_ref = f"util.attribute_change_type.types.{game_entity_name}Construct"
        change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
        contruct_hp_raw_api_object.add_raw_member("type",
                                                  change_type,
                                                  attr_effect_parent)

        # Total change time
        change_time = constructable_line.get_head_unit()["creation_time"].value
        contruct_hp_raw_api_object.add_raw_member("total_change_time",
                                                  change_time,
                                                  attr_effect_parent)

        # Ignore protection
        contruct_hp_raw_api_object.add_raw_member("ignore_protection",
                                                  [],
                                                  attr_effect_parent)

        line.add_raw_api_object(contruct_hp_raw_api_object)
        contruct_hp_forward_ref = ForwardRef(line, contruct_hp_ref)
        effects.append(contruct_hp_forward_ref)

    return effects

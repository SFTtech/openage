# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create resistances for constructing buildings.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup


def get_construct_resistances(
    line: GenieGameEntityGroup,
    ability_ref: str
) -> list[ForwardRef]:
    """
    Creates resistances that are used for constructing (unit command: 101)

    :param line: Unit/Building line that gets the ability.
    :type line: ...dataformat.converter_object.ConverterObjectGroup
    :param ability_ref: Reference of the ability raw API object the effects are added to.
    :type ability_ref: str
    :returns: The forward references for the effects.
    :rtype: list
    """
    current_unit_id = line.get_head_unit_id()
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    resistances = []

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    progress_resistance_parent = "engine.resistance.continuous.time_relative_progress.TimeRelativeProgressChange"
    progress_construct_parent = "engine.resistance.continuous.time_relative_progress.type.TimeRelativeProgressIncrease"
    attr_resistance_parent = "engine.resistance.continuous.time_relative_attribute.TimeRelativeAttributeChange"
    attr_construct_parent = "engine.resistance.continuous.time_relative_attribute.type.TimeRelativeAttributeIncrease"

    # Progress
    resistance_ref = f"{ability_ref}.ConstructProgress"
    resistance_raw_api_object = RawAPIObject(resistance_ref,
                                             "ConstructProgress",
                                             dataset.nyan_api_objects)
    resistance_raw_api_object.add_raw_parent(progress_construct_parent)
    resistance_location = ForwardRef(line, ability_ref)
    resistance_raw_api_object.set_location(resistance_location)

    # Type
    type_ref = f"util.construct_type.types.{game_entity_name}Construct"
    change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
    resistance_raw_api_object.add_raw_member("type",
                                             change_type,
                                             progress_resistance_parent)

    line.add_raw_api_object(resistance_raw_api_object)
    resistance_forward_ref = ForwardRef(line, resistance_ref)
    resistances.append(resistance_forward_ref)

    # Stacking of villager construction times
    construct_property = dataset.pregen_nyan_objects["resistance.property.types.BuildingConstruct"].get_nyan_object(
    )
    properties = {
        api_objects["engine.resistance.property.type.Stacked"]: construct_property
    }

    # Add the predefined property
    resistance_raw_api_object.add_raw_member("properties",
                                             properties,
                                             "engine.resistance.Resistance")

    # Health
    resistance_ref = f"{ability_ref}.ConstructHP"
    resistance_raw_api_object = RawAPIObject(resistance_ref,
                                             "ConstructHP",
                                             dataset.nyan_api_objects)
    resistance_raw_api_object.add_raw_parent(attr_construct_parent)
    resistance_location = ForwardRef(line, ability_ref)
    resistance_raw_api_object.set_location(resistance_location)

    # Type
    type_ref = f"util.attribute_change_type.types.{game_entity_name}Construct"
    change_type = dataset.pregen_nyan_objects[type_ref].get_nyan_object()
    resistance_raw_api_object.add_raw_member("type",
                                             change_type,
                                             attr_resistance_parent)

    # Stacking of villager construction HP increase
    construct_property = dataset.pregen_nyan_objects["resistance.property.types.BuildingConstruct"].get_nyan_object(
    )
    properties = {
        api_objects["engine.resistance.property.type.Stacked"]: construct_property
    }

    # Add the predefined property
    resistance_raw_api_object.add_raw_member("properties",
                                             properties,
                                             "engine.resistance.Resistance")

    line.add_raw_api_object(resistance_raw_api_object)
    resistance_forward_ref = ForwardRef(line, resistance_ref)
    resistances.append(resistance_forward_ref)

    return resistances

# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for deleting a unit via the PassiveTransformTo ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def delete_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds a PassiveTransformTo ability to a line that is used to make entities deletable,
    i.e. die on command.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.Delete"
    ability_raw_api_object = RawAPIObject(ability_ref, "Delete", dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.ActiveTransformTo")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    # Ability properties
    properties = {}

    # Animation
    ability_animation_id = current_unit["dying_graphic"].value
    if ability_animation_id > -1:
        property_ref = f"{ability_ref}.Animated"
        property_raw_api_object = RawAPIObject(property_ref,
                                               "Animated",
                                               dataset.nyan_api_objects)
        property_raw_api_object.add_raw_parent("engine.ability.property.type.Animated")
        property_location = ForwardRef(line, ability_ref)
        property_raw_api_object.set_location(property_location)

        line.add_raw_api_object(property_raw_api_object)

        # Use the animation from Death ability
        animations_set = []
        animation_ref = f"{game_entity_name}.Death.DeathAnimation"
        animation_forward_ref = ForwardRef(line, animation_ref)
        animations_set.append(animation_forward_ref)
        property_raw_api_object.add_raw_member("animations", animations_set,
                                               "engine.ability.property.type.Animated")

        property_forward_ref = ForwardRef(line, property_ref)
        properties.update({
            api_objects["engine.ability.property.type.Animated"]: property_forward_ref
        })

    # Diplomacy settings
    property_ref = f"{ability_ref}.Diplomatic"
    property_raw_api_object = RawAPIObject(property_ref,
                                           "Diplomatic",
                                           dataset.nyan_api_objects)
    property_raw_api_object.add_raw_parent("engine.ability.property.type.Diplomatic")
    property_location = ForwardRef(line, ability_ref)
    property_raw_api_object.set_location(property_location)

    line.add_raw_api_object(property_raw_api_object)

    diplomatic_stances = [dataset.nyan_api_objects["engine.util.diplomatic_stance.type.Self"]]
    property_raw_api_object.add_raw_member("stances", diplomatic_stances,
                                           "engine.ability.property.type.Diplomatic")

    property_forward_ref = ForwardRef(line, property_ref)
    properties.update({
        api_objects["engine.ability.property.type.Diplomatic"]: property_forward_ref
    })

    ability_raw_api_object.add_raw_member("properties",
                                          properties,
                                          "engine.ability.Ability")

    # Transform time
    # Use the time of the dying graphics
    if ability_animation_id > -1:
        dying_animation = dataset.genie_graphics[ability_animation_id]
        death_time = dying_animation.get_animation_length()

    else:
        death_time = 0.0

    ability_raw_api_object.add_raw_member("transform_time",
                                          death_time,
                                          "engine.ability.type.ActiveTransformTo")

    # Target state (reuse from Death)
    target_state_ref = f"{game_entity_name}.Death.DeadState"
    target_state_forward_ref = ForwardRef(line, target_state_ref)
    ability_raw_api_object.add_raw_member("target_state",
                                          target_state_forward_ref,
                                          "engine.ability.type.ActiveTransformTo")

    # Transform progress (reuse from Death)
    progress_ref = f"{game_entity_name}.Death.DeathProgress"
    progress_forward_ref = ForwardRef(line, progress_ref)
    ability_raw_api_object.add_raw_member("transform_progress",
                                          [progress_forward_ref],
                                          "engine.ability.type.ActiveTransformTo")

    line.add_raw_api_object(ability_raw_api_object)

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref

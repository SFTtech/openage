# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
nyan conversion routines for the AttributeChangeTracker ability.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....service.conversion import internal_name_lookups
from .....value_object.conversion.forward_ref import ForwardRef
from .util import create_animation

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.aoc.genie_unit import GenieGameEntityGroup
    from .....value_object.conversion.forward_ref import ForwardRef


def attribute_change_tracker_ability(line: GenieGameEntityGroup) -> ForwardRef:
    """
    Adds the AttributeChangeTracker ability to a line.

    :param line: Unit/Building line that gets the ability.
    :returns: The forward reference for the ability.
    """
    current_unit = line.get_head_unit()
    current_unit_id = line.get_head_unit_id()
    dataset = line.data
    api_objects = dataset.nyan_api_objects

    name_lookup_dict = internal_name_lookups.get_entity_lookups(dataset.game_version)

    game_entity_name = name_lookup_dict[current_unit_id][0]

    ability_ref = f"{game_entity_name}.AttributeChangeTracker"
    ability_raw_api_object = RawAPIObject(ability_ref,
                                          "AttributeChangeTracker",
                                          dataset.nyan_api_objects)
    ability_raw_api_object.add_raw_parent("engine.ability.type.AttributeChangeTracker")
    ability_location = ForwardRef(line, game_entity_name)
    ability_raw_api_object.set_location(ability_location)

    line.add_raw_api_object(ability_raw_api_object)

    # Attribute
    attribute = dataset.pregen_nyan_objects["util.attribute.types.Health"].get_nyan_object()
    ability_raw_api_object.add_raw_member("attribute",
                                          attribute,
                                          "engine.ability.type.AttributeChangeTracker")

    # Change progress
    damage_graphics = current_unit["damage_graphics"].value
    progress_forward_refs = []

    # Damage graphics are ordered ascending, so we start from 0
    interval_left_bound = 0
    for damage_graphic_member in damage_graphics:
        interval_right_bound = damage_graphic_member["damage_percent"].value
        progress_ref = f"{ability_ref}.ChangeProgress{interval_right_bound}"
        progress_raw_api_object = RawAPIObject(progress_ref,
                                               f"ChangeProgress{interval_right_bound}",
                                               dataset.nyan_api_objects)
        progress_raw_api_object.add_raw_parent("engine.util.progress.Progress")
        progress_location = ForwardRef(line, ability_ref)
        progress_raw_api_object.set_location(progress_location)

        line.add_raw_api_object(progress_raw_api_object)

        # Type
        progress_raw_api_object.add_raw_member("type",
                                               api_objects["engine.util.progress_type.type.AttributeChange"],
                                               "engine.util.progress.Progress")

        # Interval
        progress_raw_api_object.add_raw_member("left_boundary",
                                               interval_left_bound,
                                               "engine.util.progress.Progress")
        progress_raw_api_object.add_raw_member("right_boundary",
                                               interval_right_bound,
                                               "engine.util.progress.Progress")

        # Progress properties
        properties = {}
        # =====================================================================================
        # AnimationOverlay property
        # =====================================================================================
        progress_animation_id = damage_graphic_member["graphic_id"].value
        if progress_animation_id > -1:
            property_ref = f"{progress_ref}.AnimationOverlay"
            property_raw_api_object = RawAPIObject(property_ref,
                                                   "AnimationOverlay",
                                                   dataset.nyan_api_objects)
            property_raw_api_object.add_raw_parent(
                "engine.util.progress.property.type.AnimationOverlay")
            property_location = ForwardRef(line, progress_ref)
            property_raw_api_object.set_location(property_location)

            line.add_raw_api_object(property_raw_api_object)

            # Animation
            animations_set = []
            animation_forward_ref = create_animation(
                line,
                progress_animation_id,
                property_ref,
                "Idle",
                f"idle_damage_override_{interval_right_bound}_"
            )
            animations_set.append(animation_forward_ref)
            property_raw_api_object.add_raw_member("overlays",
                                                   animations_set,
                                                   "engine.util.progress.property.type.AnimationOverlay")

            property_forward_ref = ForwardRef(line, property_ref)
            properties.update({
                api_objects["engine.util.progress.property.type.AnimationOverlay"]: property_forward_ref
            })

        progress_raw_api_object.add_raw_member("properties",
                                               properties,
                                               "engine.util.progress.Progress")

        progress_forward_refs.append(ForwardRef(line, progress_ref))
        interval_left_bound = interval_right_bound

    ability_raw_api_object.add_raw_member("change_progress",
                                          progress_forward_refs,
                                          "engine.ability.type.AttributeChangeTracker")

    ability_forward_ref = ForwardRef(line, ability_raw_api_object.get_id())

    return ability_forward_ref
